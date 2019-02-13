#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <search.h>
#include "colors.h"
#include "bdd.h"
#include "parse.h"

// functions
//	not(x)		= (x -> 0, 1)
//	and(x, y)	= (x -> (y -> 1, 0), 0)
//	or(x, y)	= (x -> 1, (y -> 1, 0))
//	imp(x, y)	= (x -> (y -> 1, 0), 1)
//	equiv(x, y)	= (x -> (y -> 1, 0), (y -> 0, 1))

#define BDDErr(format, ...) \
	fprintf(stderr, KRED "BDD Error: " format KRST, ##__VA_ARGS__)

typedef struct {
	int i; // var(u)
	int l; // low(u)
	int h; // high(u)
} mk_node;

typedef struct {
	mk_node *table;
	int max;
} t_table_t;

// T table functions
static t_table_t T;
static void init_t_table(int numXs) {
	T.table = calloc(NUM_TABLE_ENTRIES, sizeof(T.table));
	T.table[0].i = numXs + 1;
	T.table[1].i = numXs + 1;
	T.max = 2;
}

static int add_t_table(int i, int l, int h) {
	// set values for table[0,1]
	int u = T.max++;
	T.table[u].i = i;
	T.table[u].l = l;
	T.table[u].h = h;
	return u;
}

static void free_t_table(void) {
	free(T.table);
}

// H table functions
#define KEY_LEN 256

static char keys[NUM_TABLE_ENTRIES][KEY_LEN];
static int numKeys;

static void init_h_table(void) {
	hcreate(NUM_TABLE_ENTRIES);
	numKeys = 0;
	for (int i = 0; i < NUM_TABLE_ENTRIES; i++) {
		memset(keys[i], 0, KEY_LEN);
	}
}

static bool member_h_table(int i, int l, int h) {
	ENTRY e, *ep;
	char key[KEY_LEN] = {0};
	sprintf(key, "%d,%d,%d", i, l, h);
	e.key = key;
	ep = hsearch(e, FIND);
	return (ep == NULL) ? false : true;
}

static int lookup_h_table(int i, int l, int h) {
	ENTRY e, *ep;
	char key[KEY_LEN] = {0};
	sprintf(key, "%d,%d,%d", i, l, h);
	e.key = key;
	ep = hsearch(e, FIND);
	return (ep == NULL) ? -1 : (int)(long)(ep->data);
}

static void insert_h_table(int i, int l, int h, int u) {
	ENTRY e, *ep;
	char *key = keys[numKeys++];
	sprintf(key, "%d,%d,%d", i, l, h);
	e.key = key;
	e.data = (void *)(long) u;
	ep = hsearch(e, ENTER);
	if (ep == NULL) {
		BDDErr("H entry failed\n");
	}
}

static void free_h_table(void) {
	hdestroy();
}

// accessible init functions
void init_bdd(int numXs) {
	init_t_table(numXs);
	init_h_table();
}

void free_bdd(void) {
	free_h_table();
	free_t_table();
}

// other available functions
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

int __BUILD(x_val_t *xvals, int i) {
	if (i > T.table[0].i - 1) {
		x_val_t eval = eval_expr(xvals);
		if (eval == X_VAL_0) {
			return 0;
		} else if (eval == X_VAL_1) {
			return 1;
		} else {
			BDDErr("Build error, eval returned X_VAL_X");
			return -1;
		}
	} else {
		int v[2];
		for (int j = 0; j < 2; j++) {
			xvals[i] = (j == 0) ? X_VAL_0 : X_VAL_1;
			v[j] = __BUILD(xvals, i + 1);
		}
		return MK(i, v[0], v[1]);
	}
}

// BUILD
// Builds a BDD
// args: N/A
// returns: 0 if contradiction, 1 if tautology, else number of variables + 1
int BUILD(void) {
	x_val_t *xvals = calloc(get_expr_size() + 1, sizeof(*xvals));
	return __BUILD(xvals, 1);
}

// printBDD
// prints TTable and HTable
// args: N/A
// returns: N/A
void printBDD(void) {
	printf("Printing T table:\n");
	printf("u\ti\tl\th\n");
	for (int u = 0; u < T.max; u++) {
		printf("%d\t%d\t%d\t%d\n", u, T.table[u].i, T.table[u].l, T.table[u].h);
	}

	ENTRY e, *ep;
	char *key;
	printf("Printing H table:\n");
	printf("key\tu\n");
	for (int u = 0; u < numKeys; u++) {
		key = keys[u];
		e.key = key;
		ep = hsearch(e, FIND);
		printf("%s\t%d\n", key, (ep == NULL) ? 0 : (int)(long)(ep->data));
	}
}