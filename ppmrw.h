#ifndef PPMRW_HEADER_INCLUDED
#define PPMRW_HEADER_INCLUDED
#include <stdio.h>

typedef struct {
	unsigned char r,g,b;
} Pixel;

typedef struct{
	int width, height, max_value;
	Pixel* buffer;
} Image;

void write_file(FILE* , Image* , int );
#endif
