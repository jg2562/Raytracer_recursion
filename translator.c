#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include <parser.h>


Metafield* get_field_by_name(Metaobject* obj, char* name){
	Metafield* curr_field = obj->fields;
	while (curr_field != NULL){
		if (strcmp(curr_field->field_name, name) == 0){
			return curr_field;
		}
		curr_field = curr_field->next;
	}
	return NULL;
}

Metafield* require_field_by_name(Metaobject* obj, char* name){
	Metafield* field = get_field_by_name(name);
	if (field == NULL){
		report_error_on_line("Required %s missing in %s", line);
		printf("Error: ", name, obj->type, obj->begin_line);
		exit(1);
	}

	return field;
}

void get_quadric_coefs(Metaobject* obj, double* coefs){
	Metafield* curr_field = obj->fields;
	while (curr_field != NULL){
		char* f_name = curr_field->field_name; 
		if (f_name[1] == 0 && (f_name[0] >= 65 && f_name[0] < 75) ){
			coefs[f_name[0] - 65] = curr_field->scalar;
		}
		curr_field = curr_field->next;
	}
}

int check_color(double* color){
	return color[0] < 0 || color[0] > 1 ||
		color[1] < 0 || color[1] > 1 ||
		color[2] < 0 || color[2] > 1; 
}

void check_camera(Camera* c, Metaobject* m_obj){
	if (c->width < 0)
		report_error_on_line("Invalid width in camera", m_obj->line_num);
	if (c->height < 0)
		report_error_on_line("Invalid height in camera", m_obj->line_num);
}

void check_sphere(Sphere* s, Metaobject* m_obj){

	if (check_color(s->diff_color))
		report_error_on_line("Invalid diffuse color on sphere", m_obj->line_num);

	if (check_color(s->spec_color))
		report_error_on_line("Invalid specular color on sphere", m_obj->line_num);
	
}

void check_plane(Plane* p, Metaobject* m_obj){

}

void
	
}

void check_light(Light* l, Metaobject* m_obj){
	
}

Camera* get_camera(Metaobject* m_obj){
	Camera* c = malloc(sizeof(Camera));
	c->id = 1;
	c->width = require_field_by_name(m_obj, "width")->scalar;
	c->height = require_field_by_name(m_obj, "height")->scalar;
	return c;
}

Sphere* get_sphere(Metaobject* m_obj){
	Sphere* s = malloc(sizeof(Sphere));
	s->id = 2;
	s->pos = require_field_by_name(m_obj, "position")->vector;
	s->diff_color = require_field_by_name(m_obj, "diffuse_color")->vector;
	s->specular_color = require_field_by_name(m_obj, "specular_color")->vector;
	s->radius = require_field_by_name(m_obj, "radius")->scalar;
	return s;
}

Plane* get_plane(Metaobject* m_obj){
	Plane* p = malloc(sizeof(Plane));
	p->id = 3;
	p->pos = require_field_by_name(m_obj, "position")->vector;
	p->diff_color = require_field_by_name(m_obj, "diffuse_color")->vector;
	p->specular_color = require_field_by_name(m_obj, "specular_color")->vector;
	p->normal = require_field_by_name(m_obj, "normal")->vector;
	return p;
}

Quadric* get_quadric(Metaobject* m_obj){
	Quadric* q = malloc(sizeof(Quadric));
	q->id = 4;
	q->pos = require_field_by_name(m_obj, "position")->vector;
	q->diff_color = require_field_by_name(m_obj, "diffuse_color")->vector;
	q->specular_color = require_field_by_name(m_obj, "specular_color")->vector;
	double coefs[10] = {0};
	get_quadric_coefs(obj,coefs);
	for (int i = 0; i < 10; i++)
		*(&(q->A) + i) = coefs[i];
	return q;
}

Light* get_light(Metaobject* m_obj){
	Light* l = malloc(sizeof(Light));
	l->id = 5;
	l->pos = require_field_by_name(m_obj, "position")->vector;
	l->color = require_field_by_name(m_obj, "color")->vector;
	l->r_a0 = require_field_by_name(m_obj, "radial-a0")->scalar;
	l->r_a1 = require_field_by_name(m_obj, "radial-a1")->scalar;
	l->r_a2 = require_field_by_name(m_obj, "radial-a2")->scalar;
	Metafield* sub;
	
	sub = get_field_by_name(obj, "direction");
	if (sub == NULL)
		l->dir = NULL;
	else
		l->dir = sub->vector;
	
	sub = get_field_by_name(obj, "theta");
	if (sub == NULL)
		l->theta = 1;
	else
		l->theta = cos(sub->scalar * M_PI / 180);
	
	sub = get_field_by_name(obj, "angular_a0");
	if (sub == NULL)
		l->ang_a0 = NULL;
	else
		l->ang_a0 = 0;

	return l;
}

Object* get_object(Metaobject* m_obj){
	Metafield* field;
	Object* obj = NULL;
	get_field_by_name(&field, m_obj, "type");
	
	if (field == NULL)
		report_error("No type for object");
	
	if (field->id == STRING){
		if (strcmp(field->string, "plane") == 0)
			obj = (Object*) get_plane(m_obj);
		else if (strcmp(field->string, "sphere") == 0)
			obj = (Object*) get_sphere(m_obj);
		else if (strcmp(field->string, "quadric") == 0)
			obj = (Object*) get_quadric(m_obj);
		else
			report_error("Given bad object type");
	} else
		report_error("Object type wasn't string");

	return obj;
}

Scene* translate_scene(Metaobject* obj) {
	return NULL;

}
