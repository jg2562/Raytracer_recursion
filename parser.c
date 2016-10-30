#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "structures.h"
#include "3dmath.h"
#include "parser.h"
#include "utils.h"

static int line = 1;

static void report_parsing_error(const char* msg){
	report_error_on_line(msg, line);
}

// next_c() wraps the getc() function and provides error checking and line
// number maintenance
static int get_c(FILE* json) {
	int c = fgetc(json);
#ifdef DEBUG1
	printf("next_c: '%c'\n", c);
#endif
	if (c == '\n') {
		line += 1;
	}
	// Try to prevert this error message
	if (c == EOF) {
		fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
		exit(1);
	}
	
	return c;
}

static void unget_c(char c, FILE* json){
	if (c == '\n'){
		line -= 1;
	}
	ungetc(c,json);
	
}

/*
static void peek_c(FILE* json){
	int c = get_c(json);
	
	#ifdef DEBUG2
	printf("next_c: '%c'\n", c);
	#endif
	
	unget_c(c, json);
}
*/

// expect_c() checks that the next character is d.  If it is not it emits
// an error.
static int expect_c(FILE* json, char c) {
	char d = get_c(json);
	if (c == d) return 1;
	else return 0;
	// fprintf(stderr, "Error: Expected '%c' on line %d.\n", d, line);
	// exit(1);    
}


static void require_c(FILE* json, char c, const char* error_msg){
	if (expect_c(json, c))
		return;
	else
		report_parsing_error(error_msg);
}

// skip_ws() skips white space in the file.
void skip_ws(FILE* json) {
	int c = get_c(json);
	while (isspace(c)) {
		c = get_c(json);
	}
	unget_c(c, json);
}

Metafield* new_field(){
	Metafield* field = malloc(sizeof(Metafield));
	memset(field, 0, sizeof(*field));
	return field;
}

// next_string() gets the next string from the file handle and emits an error
// if a string can not be obtained.
char* next_string(FILE* json) {
	char buffer[129];
	char c;
	require_c(json, '"', "Expected string"); 
	c = get_c(json);
	int i = 0;
	while (c != '"') {
		if (i >= 128) {
			report_parsing_error("Strings longer than 128 characters in length are not supported");
		} else if (c == '\\') {
			report_parsing_error("Strings with escape codes are not supported");
		} else if (c < 32 || c > 126) {
			report_parsing_error("Strings may contain only ascii characters");
		}
		buffer[i] = c;
		i += 1;
		c = get_c(json);
	}
	buffer[i] = 0;
	return strdup(buffer);
}

double next_number(FILE* json) {
	double value;
	char c = fscanf(json, "%lf", &value);
	if (c == EOF){
		report_parsing_error("Unexpected error while parsing double");
	}else if (c != 1){
		report_parsing_error("Invalid Number");
	}
	return value;
}

double* next_vector(FILE* json) {
	double* v = malloc(3*sizeof(double));
	require_c(json, '[', "Expected '[' to begin vector");
	skip_ws(json);
	v[0] = next_number(json);
	skip_ws(json);
	require_c(json, ',', "Unexpected ',' when parsing vector");
	skip_ws(json);
	v[1] = next_number(json);
	skip_ws(json);
	require_c(json, ',', "Unexpected ',' when parsing vector");
	skip_ws(json);
	v[2] = next_number(json);
	skip_ws(json);
	require_c(json, ']', "Expected ']' to close vector");
	return v;
}

void field_to_string(Metafield* field, char* string){
	field->id = STRING;
	field->val.string = string;
}

void field_to_vector(Metafield* field, double* vector){
	field->id = VECTOR;
	field->val.vector = vector;
}

void field_to_scalar(Metafield* field, double scalar){
	field->id = SCALAR;
	field->val.scalar = scalar;
}

void read_key(Metafield* field, FILE* json){
	field->line_num = line;
	field->field_name = next_string(json);
}

void read_value(Metafield* field, FILE* json){
	if (expect_c(json, '"')){
		field->id = STRING;
		field->val.string = next_string(json);
	} else if (expect_c(json, '[')){
		field->id = VECTOR;
		field->val.vector = next_vector(json);
	} else {
		field->id = SCALAR;
		field->val.scalar = next_number(json);
	}
}

void read_field(Metafield* field, FILE* json){
	skip_ws(json);
	read_key(field, json);
	skip_ws(json);
	require_c(json, ':', "Expected ':' while parsing field");
	skip_ws(json);
	read_value(field, json);
	skip_ws(json);
	if(expect_c(json,','))
		get_c(json);
}

Metaobject* read_object(Metaobject* prev, FILE* json){
	skip_ws(json);
	if (!expect_c(json, '{')){
		return NULL;
	}

	printf("Object Read\n");
	Metaobject* obj = malloc(sizeof(Metaobject));
	skip_ws(json);

	obj->begin_line = line;
	prev->next = obj;
	obj->next = NULL;
	
	Metafield* field;
	Metafield* n_field;
	while(!expect_c(json, '}')){
			
		n_field = new_field();
		if (field == NULL)
			obj->fields = n_field;
		else
			field->next = n_field;
		
		field = n_field;

		read_field(field, json);
		skip_ws(json);
	}
	
	field->next = NULL;
	
	if (expect_c(json, ','))
		get_c(json);
	
	return obj;
}

Metaobject* read_scene(char* filename)  {
	FILE* json = fopen(filename, "r");
	skip_ws(json);
	require_c(json, '[', "Expected '[' to begin file");
	Metaobject* first = NULL;
	Metaobject* obj = NULL;
	do{
		printf("Begin reading\n");
		if (first == NULL)
			first = obj;
			
		skip_ws(json);
		obj = read_object(obj, json);
	} while(obj != NULL);
	skip_ws(json);
	require_c(json, ']', "Expected ']' to end file");
	return first;
}
