#include <stdbool.h>
#include "colors.h"
#include "bdd.h"

// functions
//	not(x)		= (x -> 0, 1)
//	and(x, y)	= (x -> (y -> 1, 0), 0)
//	or(x, y)	= (x -> 1, (y -> 1, 0))
//	imp(x, y)	= (x -> (y -> 1, 0), 1)
//	equiv(x, y)	= (x -> (y -> 1, 0), (y -> 0, 1))

#define BDDErr(format, ...) \
	fprintf(stderr, KRED "BDD Error: " format KRST, ##__VA_ARGS__)

// init_t_table
// creates a T table and allocates memory
// args: bdd to init, number of unqiue x values
// returns: N/A
static void init_t_table(bdd_t *bdd) {
	bdd->T.table = calloc(NUM_TABLE_ENTRIES, sizeof(bdd->T.table));
	bdd->T.table[0].i = bdd->expr->numXs + 1;
	bdd->T.table[1].i = bdd->expr->numXs + 1;
	bdd->T.max = 2;
}

// add_t_table
// adds values to the T table
// args: bdd, x#, low node, high node
// returns: node #
static int add_t_table(bdd_t *bdd, int i, int l, int h) {
	// set values for table[0,1]
	int u = bdd->T.max++;
	bdd->T.table[u].i = i;
	bdd->T.table[u].l = l;
	bdd->T.table[u].h = h;
	return u;
}

// free_t_table
// deallocates memory for T table
// args: bdd
// returns: N/A
static void free_t_table(bdd_t *bdd) {
	free(bdd->T.table);
}

// H table functions

// init_h_table
// initializes the hash table and list of keys
// args: bdd
// returns: N/A
static void init_h_table(bdd_t *bdd) {
	bdd->htab = calloc(1, sizeof(*(bdd->htab)));
	hcreate_r(NUM_TABLE_ENTRIES, bdd->htab);
	bdd->keys = calloc(NUM_TABLE_ENTRIES, sizeof(*(bdd->keys)));
	for (int i = 0; i < NUM_TABLE_ENTRIES; i++) {
		bdd->keys[i] = calloc(KEY_LEN, sizeof(*(bdd->keys[i])));
	}
	bdd->numKeys = 0;
}

// member_h_table
// checks if a key is in the hash table
// args: x#, low node, high node
// returns: true if exists, else false
static bool member_h_table(bdd_t *bdd, int i, int l, int h) {
	int ret;
	ENTRY e, *ep;
	char key[KEY_LEN] = {0};
	sprintf(key, "%d,%d,%d", i, l, h);
	e.key = key;
	ret = hsearch_r(e, FIND, &ep, bdd->htab);
	return (ret) ? true : false;
}

// lookup_h_table
// looks up the value in the hash table
// args: x#, low node, high node
// returns: node #
static int lookup_h_table(bdd_t *bdd, int i, int l, int h) {
	int ret;
	ENTRY e, *ep;
	char key[KEY_LEN] = {0};
	sprintf(key, "%d,%d,%d", i, l, h);
	e.key = key;
	ret = hsearch_r(e, FIND, &ep, bdd->htab);
	return (ret) ? (int)(long)(ep->data) : -1;
}

// insert_h_table
// insert a new value into the hash table
// args: x #, high node, low node, node #
// returns: N/A
static void insert_h_table(bdd_t *bdd, int i, int l, int h, int u) {
	int ret;
	ENTRY e, *ep;
	char *key = bdd->keys[bdd->numKeys++];
	sprintf(key, "%d,%d,%d", i, l, h);
	e.key = key;
	e.data = (void *)(long) u;
	ret = hsearch_r(e, ENTER, &ep, bdd->htab);
	if (!ret) {
		BDDErr("H entry failed\n");
	}
}

// free_h_table
// frees the hash table
// args: N/A
// returns: N/A
static void free_h_table(bdd_t *bdd) {
	for (int i = 0; i < NUM_TABLE_ENTRIES; i++) {
		free((bdd->keys[i]));
	}
	free(bdd->keys);
	hdestroy_r(bdd->htab);
	free(bdd->htab);
}


// __BUILD
// backend to the BUILD function - as described in bdd-eap
// args: bdd, list of x values, iteration number
// returns: 0 if false, 1 if true, else number of X values + 1
static int __BUILD(bdd_t *bdd, x_val_t *xvals, int i) {
	if (i > bdd->T.table[0].i - 1) {
		x_val_t eval = eval_expr(bdd->expr, xvals);
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
			v[j] = __BUILD(bdd, xvals, i + 1);
		}
		int ret = MK(bdd, i, v[0], v[1]);
		return ret;
	}
}

// __APP
// backend to the apply function - as described in bdd-eap
// args: resulting bdd, base of tables 1 and 2, operator, indices to access
// returns: 
static int __APP(bdd_t *res, mk_node *base1, mk_node *base2, op_t op, int u1, int u2) {
	if ((u1 == 0 || u1 == 1) && (u2 == 0 || u2 == 1)) {
		switch(op) {
			case NOT:
				return (u1 == 1) ? 0 : 1;
			case AND:
				return (u1 == 1 && u2 == 1) ? 1 : 0;
			case OR:
				return (u1 == 1 || u2 == 1) ? 1 : 0;
			case IMP:
				return (u1 == 0 || (u1 == 1 && u2 == 1)) ? 1 : 0;
			case EQUIV:
				return (u1 == u2) ? 1 : 0;
			case ROOT:
				break;
		}
		return -1;
	} else if (base1[u1].i == base2[u2].i) {
		return MK(res, base1[u1].i, 
				__APP(res, base1, base2, op, base1[u1].l, base2[u2].l), 
				__APP(res, base1, base2, op, base1[u1].h, base2[u2].h));
	} else if (base1[u1].i > base2[u2].i) {
		return MK(res, base1[u1].i,
				__APP(res, base1, base2, op, base1[u1].l, u2),
				__APP(res, base1, base2, op, base1[u1].h, u2));
	} else {
		return MK(res, base2[u2].i,
				__APP(res, base1, base2, op, u1, base2[u2].l),
				__APP(res, base1, base2, op, u1, base2[u2].h));
	}
}

// accessible init functions

// init_bdd
// initializes the T and H tables
// args: number of unique X variables
// returns: N/A
void init_bdd(bdd_t *bdd, expr_t *expr) {
	bdd->expr = expr;
	init_t_table(bdd);
	init_h_table(bdd);
}

// free_bdd
// frees T and H tables
// args: N/A
// returns: N/A
void free_bdd(bdd_t *bdd) {
	free_h_table(bdd);
	free_t_table(bdd);
}

// Algorithms defined by bdd-eap

// MK
// MK algorithm as described in bdd-eap
// args: x #, low branch, high branch
// returns: node #
int MK(bdd_t *bdd, int i, int l, int h) {
	int u;
	if (l == h) {
		return l;
	} else if (member_h_table(bdd, i, l, h)) {
		u = lookup_h_table(bdd, i, l, h);
		return u;
	} else {
		u = add_t_table(bdd, i, l, h);
		insert_h_table(bdd, i, l, h, u);
		return u;
	}
}

// BUILD
// Builds a BDD
// args: N/A
// returns: 0 if contradiction, 1 if tautology, else number of variables + 1
int BUILD(bdd_t *bdd) {
	int ret;
	x_val_t *xvals = calloc(bdd->expr->numXs + 1, sizeof(*xvals));
	ret = __BUILD(bdd, xvals, 1);
	free(xvals);
	return ret;
}

// APPLY
// applies an operand to join to BDDs
// args: resulting bdd, operator, bdd1, bdd2
// returns: 0 if contradiction, 1 if tautology, otherwise number of unique x's
int APPLY(bdd_t *res, op_t op, bdd_t *bdd1, bdd_t *bdd2) {
	return __APP(res, bdd1->T.table, bdd2->T.table, op, bdd1->T.max, bdd2->T.max);
}


// other available functions

// printBDD
// prints TTable and HTable
// args: N/A
// returns: N/A
void printBDD(bdd_t *bdd) {
	printTTable(bdd);
	printHTable(bdd);	
}

// printTTable
// prints T table
// args: N/A
// returns: N/A
void printTTable(bdd_t *bdd) {
	printf("Printing T table:\n");
	printf("u\ti\tl\th\n");
	for (int u = 0; u < bdd->T.max; u++) {
		printf("%d\t%d\t%d\t%d\n", u, bdd->T.table[u].i, bdd->T.table[u].l, bdd->T.table[u].h);
	}
}

// printHTable
// print H table
// args: N/A
// returns: N/A
void printHTable(bdd_t *bdd) {
	ENTRY e, *ep;
	char *key;
	printf("Printing H table:\n");
	printf("key\tu\n");
	for (int u = 0; u < bdd->numKeys; u++) {
		key = bdd->keys[u];
		e.key = key;
		ep = hsearch(e, FIND);
		printf("%s\t%d\n", key, (ep == NULL) ? 0 : (int)(long)(ep->data));
	}
}