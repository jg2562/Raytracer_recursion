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

static inline double vector_dot(double* a, double* b) {
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

static inline void vector_cross(double* c, double* a, double* b) {
  c[0] = a[1]*b[2] - a[2]*b[1];
  c[1] = a[2]*b[0] - a[0]*b[2];
  c[2] = a[0]*b[1] - a[1]*b[0];
}
