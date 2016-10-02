#ifndef PRIM_OBJ_STRUCTS_INCLUDED
#define PRIM_OBJ_STRUCTS_INCLUDED

typedef struct {
  int id; // ID: 0
} Object;

typedef struct {
  int id; // ID: 1
  double width;
  double height;
} Camera;

typedef struct {
  int id;// ID: 2
  double* color;
  double* pos;
  double radius;
} Sphere;

typedef struct {
  int id;// ID: 3
  double* color;
  double* pos;
  double* normal;
} Plane;

typedef struct {
  int id;// ID: 4
} Quadric;

typedef struct {
  Camera* cam;
  Object** objects;
} Scene;

static inline double sqr(double v) {
	return v*v;
}

static inline void normalize(double* v) {
	double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}

#endif
