#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bdd.h"

/* functions
	not(x)		= (x -> 0, 1)
	and(x, y)	= (x -> (y -> 1, 0), 0)
	or(x, y)	= (x -> 1, (y -> 1, 0))
	imp(x, y)	= (x -> (y -> 1, 0), 1)
	equiv(x, y)	= (x -> (y -> 1, 0), (y -> 0, 1))
*/
typedef struct {
	int i; // var(u)
	int l; // low(u)
	int h; // high(u)
} mk_node;

typedef struct {
	mk_node *table;
	int max;
} t_table_t;

/* T table functions */
static t_table_t T;
void init_t_table(int size) {
	T.table = calloc(size, sizeof(T.table));
	T.table[0].i = 1;
	T.table[1].i = 1;
	T.max = 1;
}

int add(int i, int l, int h) {
	int u = ++(T.max);
	T.table[u].i = i;
	T.table[u].l = l;
	T.table[u].h = h;
	return u;
}

void free_t_table() {
	free(T.table);
}

/* H table functions */
typedef struct {
	mk_node *table;
	int max;
} h_table_t;

static h_table_t H;

void init_h_table(int size) {
	H.table = calloc(size, sizeof(H.table));
	H.max = 0;
}

bool member(int i, int l, int h) {
	int u;
	for (u = 0; u < H.max; u++) {
		if ((H.table[u].i == i) && (H.table[u].l == l) 
				&& (H.table[u].h == h)) {
			return true;
		}
	}
	return false;
}

int lookup(int i, int l, int h) {
	int u;
	for (u = 0; u < H.max; u++) {
		if ((H.table[u].i == i) && (H.table[u].l == l) 
				&& (H.table[u].h == h)) {
			return u;
		}
	}
	return H.max;
}

void free_h_table() {
	free(H.table);
}

/* other available functions */
int MK(int i, int l, int h) {
	return l;
}