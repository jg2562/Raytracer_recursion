typedef struct {
  int id;
} Object;

typedef struct {
  int id;
  double width;
  double height;
} Camera;

typedef struct {
  int id;
  double* color;
  double* pos;
  double radius;
} Sphere;

typedef struct {
  int id;
  double* color;
  double* pos;
  double* normal;
} Plane;

typedef struct {
  int id;
  double A;
  double B;
  double C;
  double D;
  double E;
  double F;
  double G;
  double H;
  double I;
  double J;
} Quadric;
