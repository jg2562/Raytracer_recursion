#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include "structures.h"
#include "ppmrw.h"
#include "parser.h"
#include "translator.h"
#include "3dmath.h"

#define SPEC_HIGHLIGHT 20
#define DIFF_FRAC 1
#define SPEC_FRAC 1
#define REFLECT_DEPTH 7

static inline int is_drawable(Object* o){
	return (o->id == 2 || o->id == 3 || o->id == 4);
}
	

/*
  Finds sphere intersection point with given ray.
  Ro: Ray origin vector.
  Rd: Ray direction vector.
  C: Sphere center position vector.
  r: Sphere radius.
*/
double sphere_intersection(double* Ro, double* Rd, Sphere* s){
	double* C = s->pos;
	double r = s->radius;
	// Finds A,B, and C for the quadratic equation
	double a = sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]);
	double b =  2 * (Rd[0] * (Ro[0] - C[0]) + Rd[1] * (Ro[1] - C[1]) + Rd[2] * (Ro[2] - C[2]));
	double c = sqr(Ro[0] - C[0]) + sqr(Ro[1] - C[1]) + sqr(Ro[2] - C[2]) - sqr(r);

	// Find the determinte for the quadratic equation
	double det = sqr(b) - 4 * a * c;
	if (det < 0) return -1;

  
	det = sqrt(det);

	// Tests which is the closest one and in front of camera.
	double t = (-b - det)/(2 * a);
	if (t > 0) return t;
	t = (-b + det)/(2 * a);
	if (t > 0) return t;

	return -1;
}

/*
  Finds plane intersection point with given ray.
  Ro: Ray origin vector.
  Rd: Ray direction vector.
  p: Plane position vector.
  n: Plane normal vector.

*/
double plane_intersection(double* Ro, double* Rd, Plane* p){
	double* pos = p->pos;
	double* n = p->normal;
	// Split plane equation into top and bottom half for understandability
	double top = (n[0] * Ro[0] - n[0] * pos[0] + n[1] * Ro[1] - n[1] * pos[1] + n[2] * Ro[2] - n[2] * pos[2]);
	double bottom = (n[0] * Rd[0] + n[1] * Rd[1] + n[2] * Rd[2]);
	double t = -1 * top / bottom;

	if (t > 0) return t;
  
	return -1;
}

/*
 * Finds the quadric intersection point.
 * Ro: Ray origin vector.
 * Rd: Ray direction vector.
 * quad: The quadric to intersect.
 */
double quadric_intersection(double* Ro, double* Rd, Quadric* quad){
	double a = (quad->A) * sqr(Rd[0]) + (quad->B) * sqr(Rd[1]) + (quad->C) * sqr(Rd[2]) + (quad->D) * (Rd[0]) * (Rd[1]) + (quad->E) * (Rd[0]) * (Rd[2]) + (quad->F) * (Rd[1]) * (Rd[2]);
    double b = 2*(quad->A) * (Ro[0] - quad->pos[0]) * (Rd[0]) + 2*(quad->B) * (Ro[1] - quad->pos[1]) * (Rd[1]) + 2*(quad->C) * (Ro[2] - quad->pos[2]) * (Rd[2]) + (quad->D) * ((Ro[0] - quad->pos[0]) * (Rd[1]) + (Ro[1] - quad->pos[1]) * (Rd[0])) + (quad->E) * (Ro[0] - quad->pos[0]) * (Rd[2]) + (quad->F) * ((Ro[1] - quad->pos[1]) * (Rd[2]) + (Rd[1]) * (Ro[2] - quad->pos[2])) + (quad->G) * (Rd[0]) + (quad->H) * (Rd[1]) + (quad->I) * (Rd[2]);
	double c = (quad->A) * sqr(Ro[0] - quad->pos[0]) + (quad->B) * sqr(Ro[1] - quad->pos[1]) + (quad->C) * sqr(Ro[2] - quad->pos[2]) + (quad->D) * (Ro[0] - quad->pos[0]) * (Ro[1] - quad->pos[1]) + (quad->E) * (Ro[0] - quad->pos[0]) * (Ro[2] - quad->pos[2]) + (quad->F) * (Ro[1] - quad->pos[1]) * (Ro[2] - quad->pos[2]) + (quad->G) * (Ro[0] - quad->pos[0]) + (quad->H) * (Ro[1] - quad->pos[1]) + (quad->I) * (Ro[2] - quad->pos[2]) + (quad->J);
	// Tests
	
	double det = sqr(b) - 4 * a * c;
	if (det < 0) return -1;
	
	det = sqrt(det);
	// Tests which is the closest one and in front of camera.
	double t = (-b - det)/(2 * a);
	if (t > 0) return t;
	t = (-b + det)/(2 * a);
	if (t > 0) return t;

	return -1;
}

/*
 * Small helper function that creates a vector along the ray at distance t
 * intersection: Output intersection
 * Ro: Ray origin vector.
 * Rd: Ray direction vector.
 * t: The distance along the ray. 
 */
static inline void get_intersection(double* intersection, double* Ro, double* Rd, double t){
	intersection[0] = Ro[0] + Rd[0] * t;
	intersection[1] = Ro[1] + Rd[1] * t;
	intersection[2] = Ro[2] + Rd[2] * t;
}

/*
 * Returns the sphere normal at the intersect.
 * normal: The output normal
 * inter: The intersection on the sphere
 * s: The sphere being intersected
 */
void get_sphere_normal(double* normal, Sphere* s, double* inter){
	vector_subtract(normal, inter, s->pos);
	normalize(normal);
}

/*
 * Returns the plane normal at the intersect.
 * normal: The output normal
 * inter: The intersection on the plane 
 * p: The plane being intersected
 */
void get_plane_normal(double* normal, Plane* p){
	memcpy(normal, p->normal, sizeof(double) * 3);
}

/*
 * Returns the quadric normal at the intersect.
 * normal: The output normal
 * inter: The intersection on the quadric
 * q: The quadric being intersected
 */
void get_quadric_normal(double* normal, Quadric* q, double* inter){
	normal[0] = 2 * q->A * (inter[0] - q->pos[0]) + q->D * (inter[1] - q->pos[1]) + q->E * (inter[2] - q->pos[2]) + q->G;
	normal[1] = 2 * q->B * (inter[1] - q->pos[1]) + q->D * (inter[0] - q->pos[0]) + q->F * (inter[2] - q->pos[2]) + q->H;
	normal[2] = 2 * q->C * (inter[2] - q->pos[2]) + q->E * (inter[0] - q->pos[0]) + q->F * (inter[1] - q->pos[1]) + q->I;
	normalize(normal);
}

void get_drawable_normal(double* normal, DrawableObject* o, double* inter){
	switch (o->id){
	case 2:
		;
		// Gets sphere parameters
		Sphere* s = (Sphere*) o;
		get_sphere_normal(normal, s, inter);
		break;
	case 3:
		;
		// Gets plane parameters
		Plane* p = (Plane*) o;
		get_plane_normal(normal, p);
		break;
	case 4:
		;
		// Gets quadric parameters
		Quadric* q = (Quadric*) o;
		get_quadric_normal(normal, q, inter);
		break;
	default:
		// Checks if invalid object was passed
		fprintf(stderr, "Attempted to get non drawable object normal with Id: %d.\n", o->id);
		exit(1);
	}
}

void get_color(double* color, double* Ro, double* Rd, Object** objects, Object* self, Light** lights, int depth);
void add_radial_attenuation(double* output_color, Light* light, double light_dist, double* light_color){
	double f_rad = 1 / (sqr(light_dist) * light->r_a2 + light_dist * light->r_a1 + light->r_a0);
	vector_scale(output_color, light_color, f_rad);
}

int add_angular_attenuation(double* output_color, Light* light, double* light_dir, double* light_color){
	double r_l_dir[3] = {0, 0, 0};
	vector_scale(r_l_dir, light_dir, -1);
	double ang_dot = vector_dot(r_l_dir, light->dir);
	// Checks if point is out of spotlight
	if (ang_dot < light->theta){
		return 1;
	} 

	// printf("ang_dot: %lf\n", pow(ang_dot, light->ang_a0));
	vector_scale(output_color, light_color, pow(ang_dot, light->ang_a0));
	return 0;
}

void add_diffuse(double* output_color, double* obj_color, double* light_color, double* light_dir, double* normal){
	double sub_color[3] = {0, 0, 0};
	double dot = max(vector_dot(normal,light_dir),0);
	vector_scale(sub_color, light_color, dot);
	vector_multiply(sub_color,sub_color, obj_color);
	vector_add(output_color, output_color, sub_color);
}

void add_specular(double* output_color, double* obj_color, double* light_color, double* light_dir, double* normal, double* ray_dir){
	double sub_color[3] = {0, 0, 0};
	double reflected_light_dir[3] = {0, 0, 0};
	vector_reflect(reflected_light_dir, light_dir, normal);
	double dot = max(0, vector_dot(reflected_light_dir, ray_dir));
	vector_scale(sub_color, light_color,pow(dot, SPEC_HIGHLIGHT));
	vector_multiply(sub_color, sub_color, obj_color);
	vector_add(output_color, output_color, sub_color);				 
}

void add_reflection(double* output_color, double* ray_dir, double* ray_inter, double* normal, Object** objects, Object* self, Light** lights, double reflectivity, int depth){

	if (reflectivity > 0){
		double Rd_reflect[3] = {0};
		vector_reflect(Rd_reflect, ray_dir, normal);
	
		double reflect_color[3] = {0};
	
		get_color(reflect_color, ray_inter, Rd_reflect, objects, self, lights, depth-1);

		vector_scale(reflect_color, reflect_color, reflectivity);
		vector_add(output_color, output_color, reflect_color);
	}

}

void add_refraction(double* output_color, double* ray_dir, double* ray_inter, double* normal, Object** objects, Object* self, Light** lights, double refractivity, double ior, int depth){

	if (refractivity > 0){
		double a[3] = {0};
		double b[3] = {0};
		vector_cross(a, normal, ray_dir);
		normalize(a);
		vector_cross(b, a, normal);

		
		double inv_ior = 1/ior;
		double sin_ior = inv_ior * vector_dot(ray_dir, b);
		double cos_ior = sqrt(1 - sqr(sin_ior));
		double refract_dir[3] = {0};
		vector_scale(refract_dir, normal, -cos_ior);
		vector_scale(b, b, sin_ior);
		vector_add(refract_dir, refract_dir, b);

		double refract_color[3] = {0};
		get_color(refract_color, ray_inter, refract_dir, objects, self, lights, depth - 1);

		vector_scale(refract_color, refract_color, refractivity);
		vector_add(output_color, output_color, refract_color);
	}
}

/*
  Casts ray to find first interaction with object.
  ray_len: The double address to return the length of the ray to.
  objects: The objects in the scene.
  self: The object to exclude during the ray casting. Can be null.
  Ro: The origin vector of the ray.
  Rd: The direction vector of the ray.
 */
Object* cast_ray(double* ray_len, Object** objects, Object* self, double* Ro, double* Rd){
	double best_t = INFINITY;
	int best_i = -1;

	// Finds the closest object on the ray
	for (int i=0; objects[i] != 0; i += 1) {
		double t = 0;
		if (self == objects[i])
			continue;
		// Determines what type of object intersection test needs to be performed
		switch(objects[i]->id) {
		case 2 :
			;
			Sphere* s = (Sphere*) objects[i];
			t = sphere_intersection(Ro, Rd,	s);
			break;
		case 3 :
			;
			Plane* p = (Plane*) objects[i];
			t = plane_intersection(Ro, Rd, p);
			break;
		case 4:
			;
			Quadric* q = (Quadric*) objects[i];
			t = quadric_intersection(Ro, Rd, q);
			break;
		default:
			fprintf(stderr, "Unsupported object during rendering with Id: %d.\n", objects[i]->id);
			exit(1);
		}
		// If distance is shorter, replace it
		if (t > 0 && t < best_t){
			best_t = t;
			best_i = i;
		}
	}

	// Returns the object with best intersection
	if (best_i != -1) {
		// Assigns ray length for returning
		*ray_len = best_t;
		return objects[best_i];
	} else {
		*ray_len = -1;
		return NULL;
	}
}

/*
 * Gets the color for the ray projected from Ro in the direction of Rd.
 * color: The color to return.
 * Ro: The ray origin.
 * Rd: The ray direction.
 * objects: The objects to check through.
 * lights: The lights to check through.
 */
void get_color(double* color, double* Ro, double* Rd, Object** objects, Object* self, Light** lights, int depth){
	double t = 0;

	// Checks if hits object, otherwise sets to black
	Object* o = cast_ray(&t, objects, self, Ro, Rd);
	if (o == NULL){
		color[0] = 0;
		color[1] = 0;
		color[2] = 0;
		return;
	}
	
	double* diff_color;
	double* spec_color;
	double reflectivity;
	double refractivity;
	double ior;
	double inter[3] = {0,0,0};
	double normal[3] = {0,0,0};
	// Global ambient color
	double AMB_COLOR[3] = {0, 0, 0};
	// Sets ambient color to object color
	color[0] = AMB_COLOR[0]; 
	color[1] = AMB_COLOR[1]; 
	color[2] = AMB_COLOR[2]; 

	if (depth <= 0)
		return;
	
	// Gets the intersection vector
	get_intersection(inter, Ro, Rd, t);

	if (is_drawable(o)){
		DrawableObject* draw_o = (DrawableObject*) o;
		diff_color = draw_o->diff_color;
		spec_color = draw_o->spec_color;
		reflectivity = draw_o->refl;
		refractivity = draw_o->refr;
		// reflectivity /= tot;
		// refractivity /= tot;
		
		ior = draw_o->ior;

		get_drawable_normal(normal, draw_o, inter);
	} else {
		fprintf(stderr, "Unsupported object during rendering with Id: %d.\n", o->id);
		exit(1);
	}
	
	// Goes through lights to find shading
	double l_dir[3] = {0,0,0};
	double Id[3] = {0,0,0};
	double Is[3] = {0,0,0};
	for (int i = 0; lights[i] != 0; i += 1){
		double new_t;
		Light* light = lights[i];
		double l_color[3];

		// Finds the vector between the intersection and the light
		vector_subtract(l_dir, light->pos, inter);
		double mag_l = mag(l_dir);
		normalize(l_dir);

		// Checks if point is in shadow from light
		cast_ray(&new_t, objects, o, inter, l_dir);
		if (new_t >= 0 && new_t < mag_l){
			continue;
		}

		// Finds the strength of the light and scales the lights color
		add_radial_attenuation(l_color, light, mag_l, light->color);

		// Calculates if the theres a spotlight
		if (light->dir != NULL && light->theta != -1 && light->ang_a0 != 0){
			printf("Spotlight\n");
			if (add_angular_attenuation(l_color, light, l_dir, l_color))
				continue;
		}
		
		// Sets up the diffuse color
		add_diffuse(Id, diff_color, l_color, l_dir, normal); 
		
		// Sets up the specular color 
		add_specular(Id, spec_color, l_color, l_dir, normal, Rd); 
	}

	
	// Adds the diffuse color
	vector_scale(Id, Id, DIFF_FRAC);
	vector_add(color, color, Id);

	// Adds the specular color
	vector_scale(Is, Is, SPEC_FRAC);
	vector_add(color, color, Is);

	// Book equations... maybe use them at some point?
	/*
	double RO = sqr((ior - 1)/(ior + 1));
	double Rd_r[3] = {0};
	vector_scale(Rd_r, Rd, -1);
	double Rd_normal_dot = vector_dot(Rd_r, normal);
	double R_theta = RO + (1 - RO) * quintuple(1 - Rd_normal_dot);
	*/
	
	vector_scale(color, color, 1 - (reflectivity + refractivity));
	add_reflection(color, Rd, inter, normal, objects, o, lights, reflectivity, depth);
	add_refraction(color, Rd, inter, normal, objects, o, lights, refractivity, ior, depth);
}

/*
  Paints the scene to an image using ray casting.
  scene: The scene to be painted.
  height: the height of the final image.
  width: the width of the final image.
*/
Image* paint_scene(Scene* scene, int height, int width) {
	Object** objects = scene->objects;
	double cx = 0;
	double cy = 0;
	double h = scene->cam->height;
	double w = scene->cam->width;

	int M = height;
	int N = width;
  
	Image* img = malloc(sizeof(Image));
	img->width = N;
	img->height = M;
	img->max_value = 255;
	img->buffer = malloc(sizeof(Pixel) * M * N);
	
	// Finds the color for each pixel and stores it
	double pixheight = h / M;
	double pixwidth = w / N;
	for (int y = 0; y < M; y += 1) {
		for (int x = 0; x < N; x += 1) {
			// Assumes ray starts at origin
			double Ro[3] = {0, 0, 0};
			// Rd = normalize(P - Ro)

			// Gets ray direction
			double Rd[3] = {
				cx - (w/2) + pixwidth * (x + 0.5),
				cy + (h/2) - pixheight * (y + 0.5),
				1
			};
			
			normalize(Rd);

			// Casts ray
			double color[3] = {0,0,0};

			get_color(color, Ro, Rd, objects, NULL, scene->lights, REFLECT_DEPTH);

			// Gets object color for pixel
			Pixel pix;
			pix.r = clamp_color(color[0]) * 255;
			pix.g = clamp_color(color[1]) * 255;
			pix.b = clamp_color(color[2]) * 255;
			img->buffer[y * N + x] = pix;
		}
	}
	return img;
}

/*
 * Frees a image struct
 * img: The image struct to be freed
 */
void free_image(Image* img){
	free(img->buffer);
	free(img);
}

/*
 * Frees a scene stuct
 * scene: The scene struct to be freed
 */
void free_scene(Scene* scene){
	Object** objects = scene->objects;
	Object* object;
	// Goes through the objects and frees their different parts
	for (int i = 0; objects[i] != 0; i += 1){
		object = objects[i];
		switch (object->id){
		case 0:
			free(object);
			break;
		case 1:
			break;
		case 2:
			;
			Sphere* s = (Sphere*) object;
			free(s->diff_color);
			free(s->spec_color);
			free(s->pos);
			free(s);
			break;
		case 3:
			;
			Plane* p = (Plane*) object;
			free(p->diff_color);
			free(p->spec_color);
			free(p->pos);
			free(p);
			break;
		case 4:
			;
			Quadric* q = (Quadric*) object;
			free(q->pos);
			free(q->diff_color);
			free(q->spec_color);
			free(q);
			break;
		default:
			fprintf(stderr, "Invalid object with id %d during freeing of scene.\n", object->id);
			exit(1);
		}
	}

	// Frees the lights in the scene and their parts
	Light** lights = scene->lights;
	Light* light;
	for (int i = 0; lights[i] != 0; i += 1){
		light = lights[i];
		free(light->color);
		free(light->pos);
		free(light->dir);
		free(light);
	}

	// Frees the arrays and the camera and then the scene itself
	free(lights);
	free(objects);
	free(scene->cam);
	free(scene);
}

int main(int argc, char* argv[]){
	// Checks for proper amount of arguments
	if (argc != 5){
		fprintf(stderr, "Proper Usage: raycast width height input.json output.ppm\n");
		exit(1);
	}

	// Checks if supplied arguments are valid
	int width, height;
	if (width = atoi(argv[1]), width <= 0){
		fprintf(stderr, "Error: Invalid width.\n");
		exit(1);
	}
	
	if (height = atoi(argv[2]), height <= 0){
		fprintf(stderr, "Error: Invalid height.\n");
		exit(1);
	}

	if (access(argv[4],W_OK) == 0){
		fprintf(stderr, "Error: Output file write access.\n");
		exit(1);
	}
	Metaobject* meta_objects = read_scene(argv[3]);
	
	// Reads in scene file
	Scene* scene = translate_scene(meta_objects);

	// Checks if scene has camera
	if (scene->cam == NULL){
		fprintf(stderr,"Error: No camera found.\n");
		exit(1);
	}

	// Paints scene into image file using raycasting
	Image* img = paint_scene(scene, height, width);

	free_scene(scene);
	scene = NULL;
	
	// Checks file opened properly
	FILE* out = fopen(argv[4],"wb");
	if (out == NULL){
		fprintf(stderr, "Error: Output file write access.\n");
		exit(1);
	}
	
	// Write image to file
	write_file(out, img, 6);
	free_image(img);
	img = NULL;
	
    fclose(out);

}
