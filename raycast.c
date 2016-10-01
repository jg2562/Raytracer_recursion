#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "structures.h"
#include "ppmrw.c"
#include "parser.c"

static inline double sqr(double v) {
	return v*v;
}


static inline void normalize(double* v) {
	double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}

double sphere_intersection(double* Ro, double* Rd, double* C, double r){
	double a = sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]);
	double b =  2 * (Rd[0] * (Ro[0] - C[0]) + Rd[1] * (Ro[1] - C[1]) + Rd[2] * (Ro[2] - C[2]));
	double c = sqr(Ro[0] - C[0]) + sqr(Ro[1] - C[1]) + sqr(Ro[2] - C[2]) - sqr(r);
	double det = sqr(b) - 4 * a * c;
	if (det < 0) return -1;
  
	det = sqrt(det);

	double t = (-b - det)/(2 * a);
	if (t > 0) return t;
	t = (-b + det)/(2 * a);
	if (t > 0) return t;

	return -1;
}

int plane_intersection(double* Ro, double* Rd, double* p, double* n){
	// a(xr - x0) + b(yr - y0,) + c(zr - z0) = 0
	double top = (n[0] * Ro[0] - n[0] * p[0] + n[1] * Ro[1] - n[1] * p[1] + n[2] * Ro[2] - n[2] * p[2]);
	double bottom = (- 1 * ((n[0] * Rd[0] + n[1] * Rd[1] + n[2] * Rd[2])));
	double t = top / bottom;

	if (t > 0) {
		return t;
	}
  
	return -1;
}

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
	
	double pixheight = h / M;
	double pixwidth = w / N;
	for (int y = 0; y < M; y += 1) {
		for (int x = 0; x < N; x += 1) {
			double Ro[3] = {0, 0, 0};
			// Rd = normalize(P - Ro)
			double Rd[3] = {
				cx - (w/2) + pixwidth * (x + 0.5),
				cy - (h/2) + pixheight * (y + 0.5),
				1
			};
			normalize(Rd);
			double best_t = INFINITY;
			int best_i = -1;
			for (int i=0; objects[i] != 0; i += 1) {
				double t = 0;

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
				default:
					fprintf(stderr, "Unsupported object during rendering with Id: %d\n", objects[i]->id);
					exit(1);
				}
				if (t > 0 && t < best_t){
					best_t = t;
					best_i = i;
				}
      
				if (best_t > 0 && best_t != INFINITY && (objects[best_i]->id == 2 || objects[best_i]->id == 3)) {
					double* color = ((Sphere*)objects[best_i])->color;
					Pixel pix;
					pix.r = color[0];
					pix.g = color[1];
					pix.b = color[2];
					img->buffer[y * N + x] = pix;
				} else {
					Pixel pix;
					pix.r = 0;
					pix.g = 0;
					pix.b = 0;
					img->buffer[y * N + x] = pix;
				}
			}
		}
	}
	return img;
}
int main(int argc, char* argv[]){
	char* filename = "setup.json";
	Scene* scene = read_scene(filename);
	if (scene->cam == NULL){
		fprintf(stderr,"Error: No camera found\n");
		exit(1);
	}

	Image* img = paint_scene(scene, 500,500);
	write_file(fopen("scene.ppm","wb"), img, 6);
}
