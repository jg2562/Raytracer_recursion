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
