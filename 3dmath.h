
#ifndef VECTOR_MATH_HEADER_INCLUDED
#define VECTOR_MATH_HEADER_INCLUDED
// Square function

static inline double quintuple(double v){
	return v * v * v * v * v;
}

static inline double sqr(double v) {
	return v*v;
}

static inline double mag(double* v){
	return sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
}

// Vector normalization
static inline void normalize(double* v) {
	double len = mag(v);
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}

static inline double clamp_color(double color){
	if (color > 1){
		return 1;
	} else if (color < 0) {
		return 0;
	} else {
		return color;
	}
}

static inline void vector_add(double* c, double* a, double* b) {
  c[0] = a[0] + b[0];
  c[1] = a[1] + b[1];
  c[2] = a[2] + b[2];
}

static inline void vector_subtract(double* c, double* a, double* b) {
  c[0] = a[0] - b[0];
  c[1] = a[1] - b[1];
  c[2] = a[2] - b[2];
}

static inline void vector_scale(double* c, double* a, double s) {
  c[0] = s * a[0];
  c[1] = s * a[1];
  c[2] = s * a[2];
}

static inline void vector_multiply(double* c, double* a, double* b){
  c[0] = a[0] * b[0];
  c[1] = a[1] * b[1];
  c[2] = a[2] * b[2];
}

static inline double vector_dot(double* a, double* b) {
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

static inline void vector_cross(double* c, double* a, double* b) {
  c[0] = a[1]*b[2] - a[2]*b[1];
  c[1] = a[2]*b[0] - a[0]*b[2];
  c[2] = a[0]*b[1] - a[1]*b[0];
}

static inline void vector_reflect(double* c, double* a, double* b){
	vector_scale(c, b, 2 * vector_dot(a, b));
	vector_subtract(c, a, c);
}

static inline double max(double a, double b){
	if (b > a)
		return b;
	else
		return a;
}

#endif
