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
void init_t_table(int numXs) {
	T.table = calloc(NUM_TABLE_ENTRIES, sizeof(T.table));
	T.table[0].i = numXs + 1;
	T.table[1].i = numXs + 1;
	T.max = 2;
}

int add_t_table(int i, int l, int h) {
	// set values for table[0,1]
	bool exists = false;
	for (int index = 0; index < T.max; index++) {
		if (T.table[index].i == i) {
			exists = true;
			break;
		}
	}
	if (!exists) {
		T.table[0].i++;
		T.table[1].i++;
	}
	int u = T.max++;
	T.table[u].i = i;
	T.table[u].l = l;
	T.table[u].h = h;
	return u;
}

void free_t_table(void) {
	free(T.table);
}

/* H table functions */
typedef struct {
	mk_node *table;
	int *u;
	int max;
} h_table_t;

static h_table_t H;

void init_h_table(void) {
	H.table = calloc(NUM_TABLE_ENTRIES, sizeof(*(H.table)));
	H.u = calloc(NUM_TABLE_ENTRIES, sizeof(*(H.u)));
	H.max = 0;
}

bool member_h_table(int i, int l, int h) {
	int index;
	for (index = 0; index < H.max; index++) {
		if ((H.table[index].i == i) && (H.table[index].l == l) 
				&& (H.table[index].h == h)) {
			return true;
		}
	}
	return false;
}

int lookup_h_table(int i, int l, int h) {
	int index;
	for (index = 0; index < H.max; index++) {
		if ((H.table[index].i == i) && (H.table[index].l == l) 
				&& (H.table[index].h == h)) {
			return H.u[index];
		}
	}
	return -1;
}

void insert_h_table(int i, int l, int h, int u) {
	int index = H.max++;
	H.table[index].i = i;
	H.table[index].l = l;
	H.table[index].h = h;
	H.u[index] = u;
}

void free_h_table(void) {
	free(H.table);
	free(H.u);
}

/* other available functions */
int MK(int i, int l, int h) {
	int u;
	if (l == h) {
		return l;
	} else if (member_h_table(i, l, h)) {
		return lookup_h_table(i, l, h);
	} else {
		u = add_t_table(i, l, h);
		insert_h_table(i, l, h, u);
		return u;
	}
}

int __BUILD(int t, int i) {
	return 1;
}

int BUILD(int t) {
	return __BUILD(t, 1);
}

void printMK(void) {
	printf("Printing T table:\n");
	printf("u\ti\tl\th\n");
	for (int u = 0; u < T.max; u++) {
		printf("%d\t%d\t%d\t%d\n", u, T.table[u].i, T.table[u].l, T.table[u].h);
	}
	printf("Printing H table:\n");
	printf("u\ti\tl\th\n");
	for (int u = 0; u < H.max; u++) {
		printf("%d\t%d\t%d\t%d\n", H.u[u], H.table[u].i, H.table[u].l, H.table[u].h);
	}
}