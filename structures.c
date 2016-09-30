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
  double color[3];
  double pos[3];
  double radius;
} Sphere;

typedef struct {
  int id;// ID: 3
  double color[3];
  double pos[3];
  double normal[3];
} Plane;

typedef struct {
  int id;// ID: 4
} Quadric;
