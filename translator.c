#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "structures.h"
#include "3dmath.h"
#include "translator.h"


Metafield* get_field_by_name(Metaobject* obj, const char* name){
	Metafield* curr_field = obj->fields;
	while (curr_field != NULL){
		if (strcmp(curr_field->field_name, name) == 0){
			return curr_field;
		}
		curr_field = curr_field->next;
	}
	return NULL;
}

Metafield* require_field_by_name(Metaobject* obj, const char* name){
	Metafield* field = get_field_by_name(obj, name);
	if (field == NULL){
		report_error_on_line("Required %s missing in %s", obj->begin_line);
	}

	return field;
}

void get_quadric_coefs(Metaobject* obj, double* coefs){
	Metafield* curr_field = obj->fields;
	while (curr_field != NULL){
		char* f_name = curr_field->field_name; 
		if (f_name[1] == 0 && (f_name[0] >= 65 && f_name[0] < 75) ){
			coefs[f_name[0] - 65] = curr_field->val.scalar;
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
		report_error_on_line("Invalid width in camera", m_obj->begin_line);
	if (c->height < 0)
		report_error_on_line("Invalid height in camera", m_obj->begin_line);
}

void check_sphere(Sphere* s, Metaobject* m_obj){

	if (check_color(s->diff_color))
		report_error_on_line("Invalid diffuse color on sphere", m_obj->begin_line);

	if (check_color(s->spec_color))
		report_error_on_line("Invalid specular color on sphere", m_obj->begin_line);

	if (s->radius <= 0)
		report_error_on_line("Invalid radius on sphere", m_obj->begin_line);
	
}

void check_plane(Plane* p, Metaobject* m_obj){

	if (check_color(p->diff_color))
		report_error_on_line("Invalid diffuse color on plane", m_obj->begin_line);

	if (check_color(p->spec_color))
		report_error_on_line("Invalid specular color on plane", m_obj->begin_line);
}

void check_quadric(Quadric* q, Metaobject* m_obj){

	if (check_color(q->diff_color))
		report_error_on_line("Invalid diffuse color on plane", m_obj->begin_line);

	if (check_color(q->spec_color))
		report_error_on_line("Invalid specular color on plane", m_obj->begin_line);
}

void check_light(Light* l, Metaobject* m_obj){

	if (l->color[0] < 0 || l->color[1] < 0 || l->color[2] < 0)
		report_error_on_line("Invalid color on light", m_obj->begin_line);

	if (l->ang_a0 < 0)
		report_error_on_line("Invalid angular_a0 on light", m_obj->begin_line);

}

void check_scene(Scene* s){
	if (s->cam == NULL)
		report_error("No camera found in scene");

	if (s->objects[0] == NULL)
		report_error("No objects found in scene");

	if (s->lights[0] == NULL)
		report_error("No lights found in scene");
}

Camera* get_camera(Metaobject* m_obj){
	Camera* c = malloc(sizeof(Camera));
	c->id = 1;
	c->width = require_field_by_name(m_obj, "width")->val.scalar;
	c->height = require_field_by_name(m_obj, "height")->val.scalar;
	check_camera(c, m_obj);
	return c;
}

Sphere* get_sphere(Metaobject* m_obj){
	Sphere* s = malloc(sizeof(Sphere));
	s->id = 2;
	s->pos = require_field_by_name(m_obj, "position")->val.vector;
	s->diff_color = require_field_by_name(m_obj, "diffuse_color")->val.vector;
	s->spec_color = require_field_by_name(m_obj, "specular_color")->val.vector;
	s->radius = require_field_by_name(m_obj, "radius")->val.scalar;
	check_sphere(s, m_obj);
	return s;
}

Plane* get_plane(Metaobject* m_obj){
	Plane* p = malloc(sizeof(Plane));
	p->id = 3;
	p->pos = require_field_by_name(m_obj, "position")->val.vector;
	p->diff_color = require_field_by_name(m_obj, "diffuse_color")->val.vector;
	p->spec_color = require_field_by_name(m_obj, "specular_color")->val.vector;
	p->normal = require_field_by_name(m_obj, "normal")->val.vector;
	normalize(p->normal);
	check_plane(p, m_obj);
	return p;
}

Quadric* get_quadric(Metaobject* m_obj){
	Quadric* q = malloc(sizeof(Quadric));
	q->id = 4;
	q->pos = require_field_by_name(m_obj, "position")->val.vector;
	q->diff_color = require_field_by_name(m_obj, "diffuse_color")->val.vector;
	q->spec_color = require_field_by_name(m_obj, "specular_color")->val.vector;
	double coefs[10] = {0};
	get_quadric_coefs(m_obj,coefs);
	for (int i = 0; i < 10; i++)
		*(&(q->A) + i) = coefs[i];
	check_quadric(q, m_obj);
	return q;
}

Light* get_light(Metaobject* m_obj){
	Light* l = malloc(sizeof(Light));
	l->id = 5;
	l->pos = require_field_by_name(m_obj, "position")->val.vector;
	l->color = require_field_by_name(m_obj, "color")->val.vector;
	l->r_a0 = require_field_by_name(m_obj, "radial-a0")->val.scalar;
	l->r_a1 = require_field_by_name(m_obj, "radial-a1")->val.scalar;
	l->r_a2 = require_field_by_name(m_obj, "radial-a2")->val.scalar;
	Metafield* sub;
	
	sub = get_field_by_name(m_obj, "direction");
	if (sub == NULL)
		l->dir = NULL;
	else{
		l->dir = sub->val.vector;
		normalize(l->dir);
	}
	
	sub = get_field_by_name(m_obj, "theta");
	if (sub == NULL)
		l->theta = 1;
	else
		l->theta = cos(sub->val.scalar * M_PI / 180);
	
	sub = get_field_by_name(m_obj, "angular_a0");
	if (sub == NULL)
		l->ang_a0 = 0;
	else
		l->ang_a0 = sub->val.scalar;

	check_light(l, m_obj);
	return l;
}

Object* get_object(Metaobject* m_obj){
	Metafield* field;
	Object* obj = NULL;
	field = get_field_by_name(m_obj, "type");
	
	if (field == NULL)
		report_error("No type for object");
	
	if (field->id == STRING){
		if (strcmp(field->val.string, "plane") == 0)
			obj = (Object*) get_plane(m_obj);
		else if (strcmp(field->val.string, "sphere") == 0)
			obj = (Object*) get_sphere(m_obj);
		else if (strcmp(field->val.string, "quadric") == 0)
			obj = (Object*) get_quadric(m_obj);
		else if (strcmp(field->val.string, "camera") == 0)
			obj = (Object*) get_camera(m_obj);
		else if (strcmp(field->val.string, "light") == 0)
			obj = (Object*) get_light(m_obj);
		else
			report_error("Given bad object type");
	} else
		report_error("Object type wasn't string");

	return obj;
}

// void free_metafield(Metafield* m_field);
void free_metafield(Metafield* m_field){
	if (m_field == NULL)
		return;
	free_metafield(m_field->next);
	free(m_field->field_name);
	if (m_field->id == STRING)
		free(m_field->val.string);
	else if (m_field->id == VECTOR)
		free(m_field->val.vector);
	free(m_field);
}

// void free_metaobject(Metaobject* m_obj);
void free_metaobject(Metaobject* m_obj){
	if (m_obj == NULL)
		return;
	free(m_obj->next);
	free_metafield(m_obj->fields);
	free(m_obj->type);
	free(m_obj);
}

Scene* translate_scene(Metaobject* m_obj) {
	Metaobject* pointer = m_obj;
	Scene* scene = malloc(sizeof(Scene));
	Object** objects = malloc(sizeof(Object*) * 128);
	Light** lights = malloc(sizeof(Light*) * 128);
	Object* obj;

	int obj_index = 0;
	int light_index = 0;
	while (pointer != NULL){
		obj = get_object(pointer);
		if (obj->id == 1)
			scene->cam = (Camera*) obj;
		else if (obj->id == 5)
			lights[light_index++] = (Light*) obj;
		else
			objects[obj_index++] = obj;
	    
		pointer = pointer->next;
	}

	free_metaobject(m_obj);

	objects[obj_index] = NULL;
	lights[light_index] = NULL;

	objects = realloc(objects, sizeof(Object*) * (obj_index + 1));
	lights = realloc(lights, sizeof(Light*) * (light_index + 1));

	scene->objects = objects;
	scene->lights = lights;

	check_scene(scene);

	return scene;
}
