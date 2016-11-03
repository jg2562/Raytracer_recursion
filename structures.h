#ifndef PRIM_OBJ_STRUCTS_INCLUDED
#define PRIM_OBJ_STRUCTS_INCLUDED
#include <math.h>
// Blank structure
typedef struct {
	int id; // ID: 0

} Object;

typedef struct {
	int id; // ID: 0
	double* pos;
	double* diff_color;
	double* spec_color;
	double refl;
	double refr;
	double ior;
} DrawableObject;

// Different object structure
typedef struct {
	int id; // ID: 1
	double width;
	double height;
} Camera;

typedef struct {
	int id;// ID: 2
	double* pos;
	double* diff_color;
	double* spec_color;
	double reflectivity;
	double refractivity;
	double ior;
	double radius;
} Sphere;

typedef struct {
	int id;// ID: 3
	double* pos;
	double* diff_color;
	double* spec_color;
	double reflectivity;
	double refractivity;
	double ior;
	double* normal;
} Plane;

// In case I get bored and want to add quadrics
typedef struct {
	int id;// ID: 4
	double* pos;
	double* diff_color;
	double* spec_color;
	double reflectivity;
	double refractivity;
	double ior;
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

typedef struct {
	int id;// ID: 5
	double* pos;
	double* color;
	double* dir;
	double r_a0;
	double r_a1;
	double r_a2;
	double ang_a0;
	double theta;
} Light;

// Basic scene file
typedef struct {
	Camera* cam;
	Object** objects;
	Light** lights;
} Scene;

/////////////META OBJECTS/////////////

typedef struct Metafield Metafield;
typedef struct Metaobject Metaobject;


enum field_id{NONE, STRING, VECTOR, SCALAR};

struct Metafield{
	enum field_id id;
	char* field_name;
	int line_num;
	union{
		double scalar;
		double* vector;
		char* string;
	} val;
	Metafield* next;
};


struct Metaobject{
	// FILL IN TYPE FIELD
	int begin_line;
	char* type;
	Metaobject* next;
	Metafield* fields;

};

#endif
