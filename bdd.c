#include <string.h>
#include <search.h>
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


#define NUM_TABLE_ENTRIES 1000000 // 1 million table entries

static mk_node T[NUM_TABLE_ENTRIES];
static int NUM_BDDS = 0;
static int MAX;

// init_t_table
// creates a T table and allocates memory
// args: bdd to init, number of unqiue x values
// returns: N/A
static void init_t_table(int numXs) {
	T[0].i = numXs + 1;
	T[1].i = numXs + 1;
	MAX = 2;
}

// add_t_table
// adds values to the T table
// args: bdd, x#, low node, high node
// returns: node #
static int add_t_table(int i, int l, int h) {
	// set values for table[0,1]
	int u = MAX++;
	T[u].i = i;
	T[u].l = l;
	T[u].h = h;
	return u;
}

// free_t_table
// deallocates memory for T table
// args: bdd
// returns: N/A
static void free_t_table(void) {
	T[0].i = 0;
	T[1].i = 0;
}

// H table functions

#define KEY_LEN 256
static char keys[NUM_TABLE_ENTRIES][KEY_LEN];
static int numKeys;

// init_h_table
// initializes the hash table and list of keys
// args: bdd
// returns: N/A
static void init_h_table(void) {
	hcreate(NUM_TABLE_ENTRIES);
	numKeys = 0;
	for (int i = 0; i < NUM_TABLE_ENTRIES; i++) {
		memset(keys[i], 0, KEY_LEN);
	}
}

// member_h_table
// checks if a key is in the hash table
// args: x#, low node, high node
// returns: true if exists, else false
static bool member_h_table(int i, int l, int h) {
	ENTRY e, *ep;
	char key[KEY_LEN] = {0};
	sprintf(key, "%d,%d,%d", i, l, h);
	e.key = key;
	ep = hsearch(e, FIND);
	return (ep == NULL) ? false : true;
}

// lookup_h_table
// looks up the value in the hash table
// args: x#, low node, high node
// returns: node #
static int lookup_h_table(int i, int l, int h) {
	ENTRY e, *ep;
	char key[KEY_LEN] = {0};
	sprintf(key, "%d,%d,%d", i, l, h);
	e.key = key;
	ep = hsearch(e, FIND);
	return (ep == NULL) ? -1 : (int)(long)(ep->data);
}

// insert_h_table
// insert a new value into the hash table
// args: x #, high node, low node, node #
// returns: N/A
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

// free_h_table
// frees the hash table
// args: N/A
// returns: N/A
static void free_h_table(void) {
	hdestroy();
}


// __BUILD
// backend to the BUILD function - as described in bdd-eap
// args: bdd, list of x values, iteration number
// returns: 0 if false, 1 if true, else number of X values + 1
static int __BUILD(expr_t *expr, x_val_t *xvals, int i) {
	if (i > T[0].i - 1) {
		x_val_t eval = eval_expr(expr, xvals);
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
			v[j] = __BUILD(expr, xvals, i + 1);
		}
		int ret = MK(i, v[0], v[1]);
		return ret;
	}
}

static int __APP_op;
// __APP
// backend to the apply function - as described in bdd-eap
// args: values u1 and u2
// returns: 0 if contradiction, 1 if tautology, else number of nodes + 2
static int __APP(int u1, int u2) {
	if ((u1 == 0 || u1 == 1) && (u2 == 0 || u2 == 1)) {
		switch(__APP_op) {
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
	} else if (T[u1].i == T[u2].i) {
		return MK(T[u1].i, 
				__APP(T[u1].l, T[u2].l), 
				__APP(T[u1].h, T[u2].h));
	} else if (T[u1].i < T[u2].i) {
		return MK(T[u1].i,
				__APP(T[u1].l, u2),
				__APP(T[u1].h, u2));
	} else {
		return MK(T[u2].i,
				__APP(u1, T[u2].l),
				__APP(u1, T[u2].h));
	}
}

static int __RES_j;
static int __RES_b;
// __APP
// backend to the restrict function - as described in bdd-eap
// args: value u
// returns: 0 if contradiction, 1 if tautology, else number of nodes + 2
static int __RES(int u) {
	if (T[u].i > __RES_j) {
		return u;
	} else if (T[u].i < __RES_j) {
		return MK(T[u].i, __RES(T[u].l), __RES(T[u].h));
	} else if (__RES_b == 0) {
		return __RES(T[u].l);
	} else {
		return __RES(T[u].h);
	}
}

// __pow2
// returns 2 to the power of n
// args: n
// returns: 2 ^ n
static int __pow2(int n) {
	int ret = 1;
	for (int i = 0; i < n; i++) {
		ret *= 2;
	}
	return ret;
}

// __count
// backend to the count algorithm - as described in bdd-eap
// args: node to start from
// returns: number of satifiable paths
static int __count(int u) {
	if (u == 0) {
		return 0;
	} else if (u == 1) {
		return 1;
	} else {
		return (__pow2(T[T[u].l].i - T[u].i - 1) * __count(T[u].l)) +
				(__pow2(T[T[u].h].i - T[u].i - 1) * __count(T[u].h));
	}
}

static bool used_entries[NUM_TABLE_ENTRIES];
// __find_used
// mark all used nodes in the tree
// args: node to start from
// returns: number of nodes visited
static int __find_used(int u) {
	int ret = (used_entries[u]++) ? 0 : 1;
	if (!(u == 0 || u == 1)) {
		ret += __find_used(T[u].l);
		ret += __find_used(T[u].h);
	}
	return ret;
}

// accessible init functions

// init_bdd
// initializes the T and H tables
// args: number of unique X variables
// returns: N/A
void init_bdd(expr_t *expr) {
	if (!NUM_BDDS) {
		MAX = 0;
		init_t_table(expr->numXs);
		init_h_table();
	}
	NUM_BDDS++;
}

// free_bdd
// frees T and H tables
// args: N/A
// returns: N/A
void free_bdd(void) {
	NUM_BDDS--;
	if (!NUM_BDDS) {
		free_h_table();
		free_t_table();
	}
}

// Algorithms defined by bdd-eap

// MK
// MK algorithm as described in bdd-eap
// args: x #, low branch, high branch
// returns: node #
int MK(int i, int l, int h) {
	int u;
	if (l == h) {
		return l;
	} else if (member_h_table(i, l, h)) {
		u = lookup_h_table(i, l, h);
		return u;
	} else {
		u = add_t_table(i, l, h);
		insert_h_table(i, l, h, u);
		return u;
	}
}

// BUILD
// Builds a BDD
// args: N/A
// returns: 0 if contradiction, 1 if tautology, else number of variables + 1
int BUILD(expr_t *expr) {
	int ret;
	x_val_t *xvals = calloc(expr->numXs + 1, sizeof(*xvals));
	ret = __BUILD(expr, xvals, 1);
	free(xvals);
	return ret;
}

// APPLY
// applies an operand to join to BDDs
// args: resulting bdd, operator, bdd1, bdd2
// returns: 0 if contradiction, 1 if tautology, otherwise number of unique x's
int APPLY(op_t op, int u1, int u2) {
	__APP_op = op;
	return __APP(u1, u2);
}

// RESTRICT
// applies an operand to join to BDDs
// args: operator, node1, node 2
// returns: 0 if contradiction, 1 if tautology, otherwise number of unique x's
int RESTRICT(int u, int j, int b) {
	__RES_j = j;
	__RES_b = b;
	return __RES(u);
}

// SATCOUNT
// determines the number of paths to satisfy the tree
// args: node to start from
// returns: number of paths that satisfy the tree
int SATCOUNT(int u) {
	return __pow2(T[u].i - 1) * __count(u);
}

// other available functions

// printTTable
// prints T table
// args: N/A
// returns: N/A
void printTTable(int u) {
	int used, printed = 0;
	memset(used_entries, 0, MAX);
	used = __find_used(u);
	printf("Printing T table:\n");
	printf("u\ti\tl\th\n");
	for (int u = 0; printed < used; u++) {
		if (used_entries[u]) {
			printf("%d\t%d\t%d\t%d\n", u, T[u].i, T[u].l, T[u].h);
			printed++;
		}
	}
}

// printHTable
// print H table
// args: N/A
// returns: N/A
void printHTable(void) {
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