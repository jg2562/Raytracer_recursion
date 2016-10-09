#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structures.h"
#include "parser.h"

int line = 1;
// next_c() wraps the getc() function and provides error checking and line
// number maintenance
int next_c(FILE* json) {
	int c = fgetc(json);
#ifdef DEBUG1
	printf("next_c: '%c'\n", c);
#endif
	if (c == '\n') {
		line += 1;
	}
	if (c == EOF) {
		fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
		exit(1);
	}
	return c;
}

void peek_c(FILE* json){
	int c = fgetc(json);
	printf("next_c: '%c'\n", c);
	ungetc(c, json);
}

// expect_c() checks that the next character is d.  If it is not it emits
// an error.
void expect_c(FILE* json, int d) {
	int c = next_c(json);
	if (c == d) return;
	fprintf(stderr, "Error: Expected '%c' on line %d.\n", d, line);
	exit(1);    
}


// skip_ws() skips white space in the file.
void skip_ws(FILE* json) {
	int c = next_c(json);
	while (isspace(c)) {
		c = next_c(json);
	}
	ungetc(c, json);
}


// next_string() gets the next string from the file handle and emits an error
// if a string can not be obtained.
char* next_string(FILE* json) {
	char buffer[129];
	int c = next_c(json);
	if (c != '"') {
		fprintf(stderr, "Error: Expected string on line %d.\n", line);
		exit(1);
	}  
	c = next_c(json);
	int i = 0;
	while (c != '"') {
		if (i >= 128) {
			fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
			exit(1);      
		}
		if (c == '\\') {
			fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
			exit(1);      
		}
		if (c < 32 || c > 126) {
			fprintf(stderr, "Error: Strings may contain only ascii characters.\n");
			exit(1);
		}
		buffer[i] = c;
		i += 1;
		c = next_c(json);
	}
	buffer[i] = 0;
	return strdup(buffer);
}

double next_number(FILE* json) {
	double value;
	char c = fscanf(json, "%lf", &value);
	if (c == EOF){
		fprintf(stderr, "Error: Unexpected end of file.\n");
		exit(1);
	}else if (c != 1){
		fprintf(stderr, "Error: Given invalid number, on line %d.\n", line);
		exit(1);
	}
	return value;
}

double* next_vector(FILE* json) {
	double* v = malloc(3*sizeof(double));
	expect_c(json, '[');
	skip_ws(json);
	v[0] = next_number(json);
	skip_ws(json);
	expect_c(json, ',');
	skip_ws(json);
	v[1] = next_number(json);
	skip_ws(json);
	expect_c(json, ',');
	skip_ws(json);
	v[2] = next_number(json);
	skip_ws(json);
	expect_c(json, ']');
	return v;
}


Scene* read_scene(char* filename)  {
	int c;
	Scene* scene = malloc(sizeof(Scene));
	scene->cam = NULL;
	Object** objects = malloc(sizeof(Object*) * 128);
	Light** lights = malloc(sizeof(Light*) * 128);
	FILE* json = fopen(filename, "r");

	if (json == NULL) {
		fprintf(stderr, "Error: Could not open file \"%s\"\n", filename);
		exit(1);
	}
  
	skip_ws(json);

	// Find the beginning of the list
	expect_c(json, '[');

	skip_ws(json);

	// Find the objects
	int o_index = 0;
	int l_index = 0;
	while (1) {
		c = fgetc(json);
		if (c == ']') {
			fprintf(stderr, "Error: This is the worst scene file EVER.\n");
			fclose(json);
			break;
		}
		if (c == '{') {
			skip_ws(json);
    
			// Parse the object
			char* key = next_string(json);
			if (strcmp(key, "type") != 0) {
				fprintf(stderr, "Error: Expected \"type\" key on line number %d.\n", line);
				exit(1);
			}

			skip_ws(json);

			expect_c(json, ':');

			skip_ws(json);

			char* value = next_string(json);
			Object* o;
			if (strcmp(value, "camera") == 0) {
				if (scene->cam != NULL){
					fprintf(stderr, "Error: Second camera found, on line number %d.\n",line);
					exit(1);
				}
				o = malloc(sizeof(Camera));
				o->id = 1;
				scene->cam = (Camera*) o;	
			} else if (strcmp(value, "sphere") == 0) {
				o = malloc(sizeof(Sphere));
				o->id = 2;
				objects[o_index] = o;
				o_index += 1;
			} else if (strcmp(value, "plane") == 0) {
				o = malloc(sizeof(Plane));
				o->id = 3;
				objects[o_index] = o;
				o_index += 1;
			} else if (strcmp(value, "quadric") == 0){
				o = malloc(sizeof(Quadric));
				o->id = 4;
				objects[o_index] = o;
				o_index += 1;
			} else if (strcmp(value, "light") == 0){
				o = malloc(sizeof(Light));
				o->id = 5;
				lights[l_index] =(Light*) o;
				l_index += 1;
			} else {
				fprintf(stderr, "Error: Unknown type, \"%s\", on line number %d.\n", value, line);
				exit(1);
			}

			skip_ws(json);

			while (1) {
				// , }
				c = next_c(json);
				if (c == '}') {
					// stop parsing this object
					break;
				} else if (c == ',') {
					// read another field
					skip_ws(json);
					char* key = next_string(json);
					skip_ws(json);
					expect_c(json, ':');
#ifdef DEBUG2
					printf("Processing Key: %s...",key);
#endif
					// Determines the key, and then applies value to correct object type
					skip_ws(json);
					if (strcmp(key, "width") == 0){
						double value = next_number(json);

						// Width error checking
						if (o->id != 1){
							fprintf(stderr, "Error: Width applied to non-camera object.\n");
							exit(1);
						} else if (value <= 0){
							fprintf(stderr, "Error: Invalid width on line %d.\n", line);
							exit(1);
						}
						
						((Camera*) o)->width = value;
					}else if(strcmp(key, "height") == 0){
						double value = next_number(json);

						// Height error checking
						if (o->id != 1){
							fprintf(stderr, "Error: Height applied to non-camera object.\n");
							exit(1);
						} else if (value <= 0){
							fprintf(stderr,"Error: Invalid height on line %d.\n", line);
							exit(1);
						}
						((Camera*) o)->height = value;
					}else if (strcmp(key, "radius") == 0) {
						double value = next_number(json);

						// Error checking for radius
						if (o->id != 2){
							fprintf(stderr, "Error: Radius applied to non-sphere object.\n");
							exit(1);
						} else if(value <= 0){
							fprintf(stderr,"Error: Invalid radius on line %d.\n", line);
							exit(1);
						}
						((Sphere*) o)->radius = value;
					} else if (strcmp(key, "color") == 0){
						double* value = next_vector(json);
						// Error checking for color
						if (value[0] < 0 || value[1] < 0 || value[2] < 0){
							fprintf(stderr,"Error: Invalid color on line %d.\n", line);
							exit(1);
						} else if (o->id != 5){
							fprintf(stderr, "Error: Basic Color applied to non-light object on line %d.\n", line);
							exit(1);
						}
						((Light*) o)->color = value;
					} else if (strcmp(key, "diffuse_color") == 0){
						double* value = next_vector(json);
						// Error checking for color
						if (value[0] < 0 || value[0] > 1 || value[1] < 0||value[1] > 1 || value[2] < 0 || value[2] > 1){
							fprintf(stderr,"Error: Invalid color on line %d.\n", line);
							exit(1);
						} else if (o->id < 2 || o-> id > 4){
							fprintf(stderr, "Error: Diffuse Color applied to non-colorable object on line %d.\n", line);
							exit(1);
						}
						((Sphere*) o)->diff_color = value;
					} else if (strcmp(key, "specular_color") == 0){
						double* value = next_vector(json);
						// Error checking for color
						if (value[0] < 0 || value[0] > 1 || value[1] < 0||value[1] > 1 || value[2] < 0 || value[2] > 1){
							fprintf(stderr,"Error: Invalid color on line %d.\n", line);
							exit(1);
						} else if (o->id < 2 || o-> id > 4){
							fprintf(stderr, "Error: Specular Color applied to non-colorable object on line %d.\n", line);
							exit(1);
						}
						((Sphere*) o)->spec_color = value;
					} else if (strcmp(key, "position") == 0){
						double* value = next_vector(json);
						if (o->id < 2 || o-> id > 5){
							fprintf(stderr, "Error: Position applied to non-positionable object on line %d.\n", line);
							exit(1);
						}
						((Sphere*) o)->pos = value;
					} else if (strcmp(key, "normal") == 0) {
						double* value = next_vector(json);
						if (o->id != 3){
							fprintf(stderr, "Error: Normal vector given to non-plane object on line %d.\n", line);
							exit(1);
						}
						normalize(value);
						((Plane*) o)->normal = value;
					} else if (strcmp(key, "direction") == 0){
						double* value = next_vector(json);
						if (o->id != 5){
							fprintf(stderr, "Error: Direction vector given to non-light object on line %d.\n", line);
							exit(1);
						}
						normalize(value);
						((Light*) o)->dir = value;
					} else if (strcmp(key, "radial-a0") == 0){
						double value = next_number(json);
						if (o->id != 5){
							fprintf(stderr, "Error: Radial-a0 applied to non-light object on line %d.\n", line);
							exit(1);
						} else if (value <= 0){
							fprintf(stderr, "Error: Radial-a0 non-positive number on line %d.\n", line);
							exit(1);
						}
						((Light*) o)->r_a0 = value;
					} else if (strcmp(key, "radial-a1") == 0){
						double value = next_number(json);
						if (o->id != 5){
							fprintf(stderr, "Error: Radial-a1 applied to non-light object on line %d.\n", line);
							exit(1);
						} else if (value <= 0){
							fprintf(stderr, "Error: Radial-a1 non-positive number on line %d.\n", line);
							exit(1);
						}
						((Light*) o)->r_a1 = value;
					} else if (strcmp(key, "radial-a2") == 0){
						double value = next_number(json);
						if (o->id != 5){
							fprintf(stderr, "Error: Radial-a2 applied to non-light object on line %d.\n", line);
							exit(1);
						} else if (value <= 0){
							fprintf(stderr, "Error: Radial-a2 non-positive number on line %d.\n", line);
							exit(1);
						}
						((Light*) o)->r_a2 = value;
					} else if (strcmp(key, "angular-a0") == 0){
						double value = next_number(json);
						if (o->id != 5){
							fprintf(stderr, "Error: Angular-a0 applied to non-light object on line %d.\n", line);
							exit(1);
						} else if (value <= 0){
							fprintf(stderr, "Error: Anglular-a0 non-positive number on line %d.\n", line);
							exit(1);
						}
						((Light*) o)->ang_a0 = value;
					} else if (key[1] == 0){
						Quadric* q = (Quadric*) o;
						double value = next_number(json);
						switch (key[0]){
						case 'A':
							q->A = value;
							break;
						case 'B':
							q->B = value;
							break;
						case 'C':
							q->C = value;
							break;
						case 'D':
							q->D = value;
							break;
						case 'E':
							q->E = value;
							break;
						case 'F':
							q->F = value;
							break;
						case 'G':
							q->G = value;
							break;
						case 'H':
							q->H = value;
							break;
						case 'I':
							q->I = value;
							break;
						case 'J':
							q->J = value;
							break;
						default:
							fprintf(stderr, "Error: Unknown property, \"%s\" on line %d.\n",
									key, line);
							exit(1);
						}
					} else {
						fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
								key, line);
						exit(1);
						//char* value = next_string(json);
					}
					skip_ws(json);
#ifdef DEBUG2
					printf("Finished Key & value.\n");
#endif
				} else {
					fprintf(stderr, "Error: Expected ',' or '}' got '%c' on line %d.\n", c, line);
					exit(1);
				}
			}
			// Assumes end of object
			skip_ws(json);
			c = next_c(json);
			if (c == ',') {
				// noop
				skip_ws(json);
			} else if (c == ']') {
				fclose(json);
				break;
			} else {
				fprintf(stderr, "Error: Expecting ',' or ']' on line %d.\n", line);
				exit(1);
			}
		} else {
			fprintf(stderr, "Error: Unexpecting char '%c' on line %d.\n",c, line);
			exit(1);
		}
	}
	objects[o_index] = NULL;
	objects = realloc(objects, sizeof(Object*) * (o_index + 1));
	scene->objects = objects;
	scene->lights = lights;
	return scene;
}

