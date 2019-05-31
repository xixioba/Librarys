#ifndef _MICRO_CORNTAB_H
#define _MICRO_CORNTAB_H

#include <stdio.h>
#include <stdint.h>

typedef struct corntab_data
{
	uint8_t weekday;
	uint16_t month;
	uint32_t monthday;
	uint32_t hour;
	uint64_t minute;
	uint64_t second;
};

typedef struct Command
{
	char *name;
	char *paras;
	int (*func)(int argc,...);
	struct corntab_data date;
};

typedef struct Func
{
	int (*func)(int argc,...);
	const char *name;
	const char *paras;
};

typedef struct Node
{
	struct Node *pre;
	struct Node *next;
	struct Command line;
}LinkList;

int Corntab_init(char * path);
int Corntab_pool();
#endif
