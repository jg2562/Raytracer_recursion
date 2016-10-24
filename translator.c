#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include <parser.h>


void get_field_by_name(Metafield** field, Metaobject* obj, char* name){
	Metafield* curr_field = obj->fields;
	while (curr_field != NULL){
		if (strcmp(curr_field->field_name, name) == 0){
			*field = curr_field;
			return;
		}
		curr_field = curr_field->next;
	}
	*field = NULL;
}

Plane* get_plane(Metaobject* m_obj){
	return NULL;
}

Sphere* get_sphere(Metaobject* m_obj){
	return NULL;
}

Quadric* get_quadric(Metaobject* m_obj){
	return NULL;
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
