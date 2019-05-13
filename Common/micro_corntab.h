#ifndef _MICRO_CORNTAB_H
#define _MICRO_CORNTAB_H
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <windows.h>


typedef struct corntab_data
{
	uint8_t weekday;
	uint32_t monthday;
	uint32_t hour;
	uint64_t minute;	
	uint64_t second;
};

typedef struct Command 
{
	const char *name;
	const char *desc;
	int (*func)(int argc,...);
	struct corntab_data date;
};

typedef struct Func
{
	int (*func)(int argc,...);
	const char *name;
	const char *paras;
};

#endif