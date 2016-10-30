#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "objects.h"
#include "structures.h"

Object* make_plane(){
	Plane* p = malloc(sizeof(Plane));
	p->id = 3;
	p->pos = NULL;
	p->diff_color = NULL;
	p->spec_color = NULL;
	p->normal = NULL;
	return (Object*) p;
}

Object* make_sphere(){
	Sphere* s = malloc(sizeof(Sphere));
	s->id = 2;
	s->pos = NULL;
	s->diff_color = NULL;
	s->spec_color = NULL;
	s->radius = -1;
	return (Object*) s;
}

Object* make_quadric(){
	Quadric* q = malloc(sizeof(Quadric));
	q->id = 4;
	q->pos = NULL;
	q->diff_color = NULL;
	q->spec_color = NULL;
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
	return (Object*) q;
}

Object* make_light(){
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
	return (Object*) l;
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
	return mobj;
}

Metafield* make_metafield(){
	
	Metafield* mf = malloc(sizeof(Metafield));
	mf->line_num = -1;
	return mf;
}
