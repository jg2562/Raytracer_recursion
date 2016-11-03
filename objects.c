#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "objects.h"
#include "structures.h"

static inline void config_drawable_object(DrawableObject* d){
	d->pos = NULL;
	d->diff_color = NULL;
	d->spec_color = NULL;
	d->refl= 0;
	d->refr= 0;
	d->ior = 0;
}

Plane* make_plane(){
	Plane* p = malloc(sizeof(Plane));
	p->id = 3;
	config_drawable_object((DrawableObject*) p);
	p->normal = NULL;
	return p;
}

Sphere* make_sphere(){
	Sphere* s = malloc(sizeof(Sphere));
	s->id = 2;
	config_drawable_object((DrawableObject*) s);
	s->radius = -1;
	return s;
}

Quadric* make_quadric(){
	Quadric* q = malloc(sizeof(Quadric));
	q->id = 4;
	config_drawable_object((DrawableObject*) q);
	q->A = -INFINITY;
	q->B = -INFINITY;
	q->C = -INFINITY;
	q->D = -INFINITY;
	q->E = -INFINITY;
	q->F = -INFINITY;
	q->G = -INFINITY;
	q->H = -INFINITY;
	q->I = -INFINITY;
	q->J = -INFINITY;
	return q;
}

Light* make_light(){
	Light* l = malloc(sizeof(Light));
	l->id = 5;
	l->pos = NULL;
	l->color = NULL;
	l->dir = NULL;
	l->r_a0 = -1;
	l->r_a1 = -1;
	l->r_a2 = -1;
	l->theta = 0;
	l->ang_a0 = 0;
	l->theta = 0;
	return l;
}

Scene* make_scene(){
	Scene* s = malloc(sizeof(Scene));
	memset(s, 0, sizeof(*s));
	return s;
}


Metaobject* make_metaobject(){
	Metaobject* mobj = malloc(sizeof(Metaobject));
	memset(mobj, 0, sizeof(*mobj));
	mobj->begin_line = -1;
	// FILL IN TYPE FIELD
	mobj->type = malloc(sizeof(char));
	*mobj->type = 0;
	return mobj;
}

Metafield* make_metafield(){
	Metafield* mf = malloc(sizeof(Metafield));
	mf->line_num = -1;
	return mf;
}
