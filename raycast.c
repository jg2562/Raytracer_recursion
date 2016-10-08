#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "structures.h"
#include "ppmrw.h"
#include "parser.h"

double quadric_intersection(double* Ro, double* Rd, Quadric* quad){
	double a = (quad->A) * sqr(Rd[0]) + (quad->B) * sqr(Rd[1]) + (quad->C) * sqr(Rd[2]) + (quad->D) * (Rd[0]) * (Rd[1]) + (quad->E) * (Rd[0]) * (Rd[2]) + (quad->F) * (Rd[1]) * (Rd[2]);
    double b = 2*(quad->A) * (Ro[0] - quad->pos[0]) * (Rd[0]) + 2*(quad->B) * (Ro[1] - quad->pos[1]) * (Rd[1]) + 2*(quad->C) * (Ro[2] - quad->pos[2]) * (Rd[2]) + (quad->D) * ((Ro[0] - quad->pos[0]) * (Rd[1]) + (Ro[1] - quad->pos[1]) * (Rd[0])) + (quad->E) * (Ro[0] - quad->pos[0]) * (Rd[2]) + (quad->F) * ((Ro[1] - quad->pos[1]) * (Rd[2]) + (Rd[1]) * (Ro[2] - quad->pos[2])) + (quad->G) * (Rd[0]) + (quad->H) * (Rd[1]) + (quad->I) * (Rd[2]);
	double c = (quad->A) * sqr(Ro[0] - quad->pos[0]) + (quad->B) * sqr(Ro[1] - quad->pos[1]) + (quad->C) * sqr(Ro[2] - quad->pos[2]) + (quad->D) * (Ro[0] - quad->pos[0]) * (Ro[1] - quad->pos[1]) + (quad->E) * (Ro[0] - quad->pos[0]) * (Ro[2] - quad->pos[2]) + (quad->F) * (Ro[1] - quad->pos[1]) * (Ro[2] - quad->pos[2]) + (quad->G) * (Ro[0] - quad->pos[0]) + (quad->H) * (Ro[1] - quad->pos[1]) + (quad->I) * (Ro[2] - quad->pos[2]) + (quad->J);

	double det = sqr(b) - 4 * a * c;
	if (det < 0) return -1;
	
	// Tests which is the closest one and in front of camera.
	double t = (-b - det)/(2 * a);
	if (t > 0) return t;
	t = (-b + det)/(2 * a);
	if (t > 0) return t;

	return -1;
}

/*
  Finds sphere intersection point with given ray.
  Ro: Ray origin vector.
  Rd: Ray direction vector.
  C: Sphere center position vector.
  r: Sphere radius.
*/
double sphere_intersection(double* Ro, double* Rd, double* C, double r){
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
double plane_intersection(double* Ro, double* Rd, double* p, double* n){
	// a(xr - x0) + b(yr - y0,) + c(zr - z0) = 0
	// Split plane equation into top and bottom half for understandability
	double top = (n[0] * Ro[0] - n[0] * p[0] + n[1] * Ro[1] - n[1] * p[1] + n[2] * Ro[2] - n[2] * p[2]);
	double bottom = (n[0] * Rd[0] + n[1] * Rd[1] + n[2] * Rd[2]);
	double t = -1 * top / bottom;

	if (t > 0) return t;
  
	return -1;
}

/*
  Casts ray to find first interaction with object.
  ray_len: The double address to return the length of the ray to.
  objects: The objects in the scene.
  Ro: The origin vector of the ray.
  Rd: The direction vector of the ray.
 */
Object* cast_ray(double* ray_len, Object** objects, double* Ro, double* Rd){

	double best_t = INFINITY;
	int best_i = -1;

	// Finds the closest object on the ray
	for (int i=0; objects[i] != 0; i += 1) {
		double t = 0;

		// Determines what type of object intersection test needs to be performed
		switch(objects[i]->id) {
		case 2 :
			;
			Sphere* s = (Sphere*) objects[i];
			t = sphere_intersection(Ro, Rd,
									s->pos,
									s->radius);
			break;
		case 3 :
			;
			Plane* p = (Plane*) objects[i];
			t = plane_intersection(Ro, Rd,
								   p->pos,
								   p->normal);
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
		ray_len = NULL;
		return NULL;
	}
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
				cx + (w/2) - pixwidth * (x + 0.5),
				cy + (h/2) - pixheight * (y + 0.5),
				1
			};
			
			normalize(Rd);

			// Casts ray
			double t;
			Object* object = cast_ray(&t, objects, Ro, Rd);

			// Gets object color for pixel
			Pixel pix;
			if (object != NULL){
				double* color = ((Sphere*)object)->color;
				pix.r = color[0] * 255;
				pix.g = color[1] * 255;
				pix.b = color[2] * 255;
			} else {
				pix.r = 0;
				pix.g = 0;
				pix.b = 0;
			}
			img->buffer[y * N + x] = pix;
		}
	}
	return img;
}
void free_image(Image* img){
	free(img->buffer);
	free(img);
}

void free_scene(Scene* scene){
	Object** objects = scene->objects;
	Object* object;
	for (int i = 0; objects[i] != 0; i += 1){
		object = objects[i];
		switch (object->id){
		case 0:
			;
			free(object);
			break;
		case 1:
			;
			break;
		case 2:
			;
			Sphere* s = (Sphere*) object;
			free(s->color);
			free(s->pos);
			free(s);
			break;
		case 3:
			;
			Plane* p = (Plane*) object;
			free(p->color);
			free(p->pos);
			free(p);
			break;
		case 4:
			;
			Quadric* q = (Quadric*) object;
			free(q);
		default:
			fprintf(stderr, "Invalid object with id %d during freeing of scene.\n", object->id);
			exit(1);
		}
	}
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
	
	// Reads in scene file
	Scene* scene = read_scene(argv[3]);

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
