#ifndef PRIM_OBJ_STRUCTS_INCLUDED
#define PRIM_OBJ_STRUCTS_INCLUDED

// Blank structure
typedef struct {
  int id; // ID: 0
} Object;

// Different object structure
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

// In case I get bored and want to add quadrics
typedef struct {
  int id;// ID: 4
} Quadric;

// Basic scene file
typedef struct {
  Camera* cam;
  Object** objects;
} Scene;

// Square function
static inline double sqr(double v) {
	return v*v;
}

// Vector normalization
static inline void normalize(double* v) {
	double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}

#endif
