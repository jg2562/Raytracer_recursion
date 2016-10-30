#ifndef OBJECTS_HEADER_INCLUDED
#define OBJECTS_HEADER_INCLUDED
#include "structures.h"
Object* make_plane();
Object* make_sphere();
Object* make_quadric();
Object* make_light();
Scene* make_scene();
Metaobject* make_metaobject();
Metafield* make_metafield();
#endif
