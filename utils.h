
#ifndef UTILS_HEADER_INCLUDED
#define UTILS_HEADER_INCLUDED
#include <string.h>

static inline void report_error(const char* msg){
	printf("Error: %s.\n", msg);
	exit(1);
}

static inline void report_error_on_line(const char* msg, int line_num){
	char n_msg[strlen(msg) + 20];
	sprintf(n_msg, "%s on line number %d", msg, line_num);
	report_error(n_msg);
}

#endif
