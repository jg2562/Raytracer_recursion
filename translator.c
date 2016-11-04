#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "structures.h"
#include "3dmath.h"
#include "translator.h"
#include "objects.h"


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
		char error_msg[strlen(name) + strlen(obj->type) + 34];
		sprintf(error_msg, "Required %s field missing in %s", name, obj->type);
		report_error_on_line(error_msg, obj->begin_line);
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

void check_drawable(DrawableObject* d, Metaobject* m_obj){
	if (check_color(d->diff_color))
		report_error_on_line("Invalid diffuse color", m_obj->begin_line);

	if (check_color(d->spec_color))
		report_error_on_line("Invalid specular color", m_obj->begin_line);

	if (d->refl < 0 || d->refl > 1)
		report_error_on_line("Invalid reflective properly", m_obj->begin_line);
		
	if (d->refr < 0 || d->refr > 1)
		report_error_on_line("Invalid refractive properly", m_obj->begin_line);

	if (d->ior < 1)
		report_error_on_line("Invalid Index of Refraction properly", m_obj->begin_line);

	if ((d->refl + d->refr) > 1)
		report_error("Reflective properly and Refractive properly greater than 1");
}

void check_camera(Camera* c, Metaobject* m_obj){
	if (c->width < 0)
		report_error_on_line("Invalid width in camera", m_obj->begin_line);
	if (c->height < 0)
		report_error_on_line("Invalid height in camera", m_obj->begin_line);
}

void check_sphere(Sphere* s, Metaobject* m_obj){
	check_drawable((DrawableObject*) s, m_obj);

	if (s->radius <= 0)
		report_error_on_line("Invalid radius on sphere", m_obj->begin_line);
	
}

void check_plane(Plane* p, Metaobject* m_obj){
	check_drawable((DrawableObject*) p, m_obj);
}

void check_quadric(Quadric* q, Metaobject* m_obj){
	check_drawable((DrawableObject*) q, m_obj);
}

void check_light(Light* l, Metaobject* m_obj){

	if (l->color[0] < 0 || l->color[1] < 0 || l->color[2] < 0)
		report_error_on_line("Invalid color on light", m_obj->begin_line);

	if (l->ang_a0 < 0)
		report_error_on_line("Invalid angular_a0 on light", m_obj->begin_line);

	if (l->theta != -1 && l->theta < 0)
		report_error_on_line("Invalid theta on light", m_obj->begin_line);

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

void fill_drawable(DrawableObject* o, Metaobject* m_obj){
	o->pos = require_field_by_name(m_obj, "position")->val.vector;
	o->diff_color = require_field_by_name(m_obj, "diffuse_color")->val.vector;
	o->spec_color = require_field_by_name(m_obj, "specular_color")->val.vector;
	o->refl = require_field_by_name(m_obj, "reflectivity")->val.scalar;
	o->refr = require_field_by_name(m_obj, "refractivity")->val.scalar;
	o->ior = require_field_by_name(m_obj, "ior")->val.scalar;
}

Sphere* get_sphere(Metaobject* m_obj){
	Sphere* s = make_sphere();
	fill_drawable((DrawableObject*) s, m_obj);
	s->radius = require_field_by_name(m_obj, "radius")->val.scalar;
	check_sphere(s, m_obj);
	return s;
}

Plane* get_plane(Metaobject* m_obj){
	Plane* p = make_plane();
	fill_drawable((DrawableObject*) p, m_obj);
	p->normal = require_field_by_name(m_obj, "normal")->val.vector;
	
	normalize(p->normal);
	check_plane(p, m_obj);
	return p;
}

Quadric* get_quadric(Metaobject* m_obj){
	Quadric* q = make_quadric();
	fill_drawable((DrawableObject*) q, m_obj);
	
	double coefs[10] = {0};
	get_quadric_coefs(m_obj,coefs);
	for (int i = 0; i < 10; i++)
		*(&(q->A) + i) = coefs[i];
	check_quadric(q, m_obj);
	return q;
}

Light* get_light(Metaobject* m_obj){
	Light* l = make_light();
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
		l->theta = -1;
	else
		l->theta = cos(sub->val.scalar * M_PI / 180);
	
	sub = get_field_by_name(m_obj, "angular-a0");
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
	Scene* scene = make_scene();
	Object** objects = malloc(sizeof(Object*) * 128);
	Light** lights = malloc(sizeof(Light*) * 128);
	Object* obj = NULL;

	int obj_index = 0;
	int light_index = 0;
	while (pointer != NULL){
		obj = get_object(pointer);
		if (obj->id == 1){
			if (scene->cam != NULL)
				report_error_on_line("Second camera found", pointer->begin_line);
			scene->cam = (Camera*) obj;
		}else if (obj->id == 5)
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
