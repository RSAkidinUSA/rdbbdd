#ifndef BDD_H
#define BDD_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include "parse.h"

#define NUM_TABLE_ENTRIES 100000 // 100 thousand table entries

typedef struct {
	int i; // var(u)
	int l; // low(u)
	int h; // high(u)
} mk_node;

typedef struct {
	mk_node *table;
	int max;
} t_table_t;


#define KEY_LEN 256

typedef struct {
	t_table_t T;
	struct hsearch_data *htab;
	char **keys;
	int numKeys;
	expr_t *expr;
} bdd_t;

// init and free functions
void init_bdd(bdd_t *bdd, expr_t *expr);
void free_bdd(bdd_t *bdd);

// other functions
int MK(bdd_t *bdd, int i, int l, int h);
int BUILD(bdd_t *bdd);
//int APPLY(op_t op, int u1, int u2);

// printing functions
void printBDD(bdd_t *bdd);
void printTTable(bdd_t *bdd);
void printHTable(bdd_t *bdd);

#endif