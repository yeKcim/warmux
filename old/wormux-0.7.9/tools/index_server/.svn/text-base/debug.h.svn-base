#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define	ALL		0
#define TRAFFIC		1
#define	MSG		2
#define	CONN		3
#define INFO		4

#define LOG_LEVEL	INFO

#ifdef DEBUG
#define DPRINT(LEVEL, ARGS...) \
{ \
	if((LEVEL) >= LOG_LEVEL ) \
	{ \
		printf("%10s,%3i : ",__FILE__,__LINE__); \
		printf(ARGS); \
		printf("\n"); \
	} \
}
#else
#define DPRINT(LEVEL, ARGS...)
#endif

#define TELL_ERROR \
{ \
	PRINT_ERROR; \
	exit(1); \
}

#define PRINT_ERROR \
{ \
	DPRINT(INFO , "%10s,%3i : ERROR! %s",__FILE__,__LINE__, strerror(errno)); \
}


#endif

