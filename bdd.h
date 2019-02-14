#ifndef BDD_H
#define BDD_H

#include <strings.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "parse.h"

typedef struct {
	int i; // var(u)
	int l; // low(u)
	int h; // high(u)
} mk_node;

// typedef struct {
// 	expr_t *expr;
// } bdd_t;

// init and free functions
void init_bdd(expr_t *expr);
void free_bdd(void);

// other functions
int MK(int i, int l, int h);
int BUILD(expr_t *expr);
int APPLY(op_t op, int u1, int u2);
int RESTRICT(int u, int j, int b);
int SATCOUNT(int u);

// functions for printing
void printTTable(int u);
void printHTable(void);

#endif