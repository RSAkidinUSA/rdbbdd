#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "parse.h"

static const char EXPR_STRS[5][10] = {
	"not(",
	"and(",
	"or(",
	"imp(",
	"equiv(",
};

/* Errors */
#define ERR_XVAL	1
#define ERR_EXPR	2
#define ERR_PAREN	3
#define ERR_COMMA	4
#define ERR_LOOP	5

#define parseErr(format, ...) \
	fprintf(stderr, "\x1B[31m" "Parsing Error: " format "\x1B[0m", ##__VA_ARGS__)

typedef struct {
	parse_node_t *root;
	int numXs;
} expr_t;

static expr_t expr;

// init_expr
// initializes the base of the expression
// args: N/A
// returns: N/A
void init_expr(void) {
	expr.numXs = 0;
	expr.root = calloc(1, sizeof(*(expr.root)));
	expr.root->op = ROOT;
}


// __del_node
// deletes all subnodes
// args: parse_node to delete subnodes from
// returns: N/A
static void __del_node(parse_node_t *node) {
	for (int i = 0; i < 2; i++) {
		if (node->valType[i] == FUNC) {
			__del_node(node->val[i]);
			free(node->val[i]);
		}
		node->valType[i] = NONE;
	}
}

// del_expr
// deletes an expression
// args: N/A
// returns: N/A
void del_expr(void) {
	expr.numXs = 0;
	__del_node(expr.root);
	free(expr.root);
}

// new_node
// checks if the expression and creates a new node
// args: node to check, function, arg number
// returns: pointer to new node or null if invalid
static parse_node_t *__new_node(parse_node_t *node, char c, int arg) {
	char str[10] = {0,};
	parse_node_t *temp;
	op_t op;
	switch(c) {
		case 'n':
		case 'N':
			op = NOT;
			break;
		case 'a':
		case 'A':
			op = AND;
			break;
		case 'o':
		case 'O':
			op = OR;
			break;
		case 'i':
		case 'I':
			op = IMP;
			break;
		case 'e':
		case 'E':
			op = EQUIV;
			break;
	}
	str[0] = EXPR_STRS[op][0];
	for (int i = 1; i < strlen(EXPR_STRS[op]); i++) {
		str[i] = getchar();
	}
	if (strncasecmp(EXPR_STRS[op], str, strlen(EXPR_STRS[op]) + 1)) {
		printf("%s, %s\n", EXPR_STRS[op], str);
		return NULL;
	} else {
		if (node->op == ROOT) {
			temp = node;
		} else {
			temp = calloc(1, sizeof(*temp));
			node->val[arg] = (void *)temp;
			node->valType[arg] = FUNC;
		}
		temp->op = op;
		return temp;
	}
}

// __parse_node
// parses an node
// args: parse_node_t tree node to modify, argument (0 or 1)
// returns: 0 if valid expression, 1 if not
static int __parse_node(parse_node_t *node, int arg) {
	parse_node_t *temp;
	int c, retval;
	unsigned long xval;
	while (arg < 2) {
		c = getchar();
		while (c == ' ' || c == '\t') {
			c = getchar();
		}
		switch(c) {
			case 'n':
			case 'N':
			case 'a':
			case 'A':
			case 'o':
			case 'O':
			case 'i':
			case 'I':
			case 'e':
			case 'E':
				if (!(temp = __new_node(node, c, arg))) {
					parseErr("Invalid function\n");
					return 0;
				}
				if ((retval = __parse_node(temp, 0))) {
					return retval;
				}
				break;
			case 'x':
			case 'X':
				xval = 0;
				c = getchar();
				if (c < '1' || c > '9') {
					parseErr("Invalid X Value\n");
					return ERR_XVAL;
				}
				while (c >= '0' && c <= '9') {
					xval *= 10;
					xval += c - '0';
					c = getchar();
				}
				if (xval == expr.numXs + 1) {
					expr.numXs++;
				}
				if (xval <= expr.numXs) {
					ungetc(c, stdin);
					node->valType[arg] = X_VAL;
					node->val[arg] = (void *) xval;
				} else {
					parseErr("X value not sequential\n");
					return ERR_XVAL;
				}
				return 0;
			case '0':
			case '1':
				node->valType[arg] = C_VAL;
				node->val[arg] = (void *) (long)(c - '0');
				return 0;
			default:
				parseErr("Invalid Expression: %c\n", c);
				return ERR_EXPR;
		}
		c = getchar();
		while (c == ' ' || c == '\t') {
			c = getchar();
		}
		if (temp->op == NOT) {
			if (c != ')') {
				parseErr("Missing Parenthesis\n");
				return ERR_PAREN;
			} else {
				return 0;
			}
		} else {
			if (c != ',') {
				parseErr("Missing Comma\n");
				return ERR_COMMA;
			}
		}
		if ((retval = __parse_node(temp, 1))) {
			return retval;
		}
		c = getchar();
		while (c == ' ' || c == '\t') {
			c = getchar();
		}
		if (c != ')') {
			parseErr("Missing Parenthesis\n");
			return ERR_COMMA;
		} else {
			return 0;
		}
	}
	parseErr("Escaped the while loop somehow\n");
	return ERR_LOOP;
}

// parse_expr
// parses an expression
// args: N/A
// return: 0 on sucess, error val on failure
int parse_expr(void) {
	return __parse_node(expr.root, 0);
}

// __print_node
// prints a node
// args: pares_node_t tree node to print
// returns: N/A
static void __print_node(parse_node_t *node) {
	int numArgs = (node->op == NOT) ? 1 : 2;
	printf("( ");
	for (int i = 0; i < numArgs; i++) {
		if (i == numArgs - 1) {
			switch(node->op) {
				case NOT:
					printf("! ");
					break;
				case AND:
					printf(" && ");
					break;
				case OR:
					printf(" || ");
					break;
				case IMP:
					printf(" -> ");
					break;
				case EQUIV:
					printf(" <-> ");
					break;
				case ROOT:
					printf(" ERROR in func value ");
					break;
			}
		}
		switch(node->valType[i]) {
			case FUNC:
				__print_node((parse_node_t *) node->val[i]);
				break;
			case X_VAL:
				printf("x");
			case C_VAL:
				printf("%ld", (long) node->val[i]);
				break;
			case NONE:
				printf(" ERROR in valType value ");
				break;
		}
	}
	printf(" )");
}


// print_expr
// prints the most recently loaded expression
// args: N/A
// returns: N/A
void print_expr(void) {
	__print_node(expr.root);
}

// get_expr_size
// returns the number of unique X values in a parsed expression
// args: N/A
// returns: number of X values
int get_expr_size(void) {
	return expr.numXs;
}

// __eval_node
// evaluates the value of a node recursively and returns it
// args: node to evaluate
// returns: evaluation
x_val_t __eval_node(parse_node_t *node, x_val_t *xvals) {
	x_val_t val[2];
	for (int i = 0; i < 2; i++) {
		switch(node->valType[i]) {
		case FUNC:
			val[i] = __eval_node((parse_node_t *)(node->val[i]), xvals);
			break;
		case X_VAL:
			val[i] = xvals[(long)(node->val[i])];
			break;
		case C_VAL:
			val[i] = node->val[i] ? X_VAL_1 : X_VAL_0;
			break;
		case NONE:
			val[i] = X_VAL_X;
			break;
		}
	}
	switch(node->op) {
		case NOT:
			if (val[0] == X_VAL_X) {
				return X_VAL_X;
			} else {
				return (val[0] == X_VAL_1) ? X_VAL_0 : X_VAL_1;
			}
		case AND:
			if (val[0] == X_VAL_0 || val[1] == X_VAL_0) {
				return X_VAL_0;
			} else if (val[0] == X_VAL_X || val[1] == X_VAL_X) {
				return X_VAL_X;
			} else {
				return X_VAL_1;
			}
		case OR:
			if (val[0] == X_VAL_1 || val[1] == X_VAL_1) {
				return X_VAL_1;
			} else if (val[0] == X_VAL_X || val[1] == X_VAL_X) {
				return X_VAL_X;
			} else {
				return X_VAL_0;
			}
		case IMP:
			if (val[0] == X_VAL_0) {
				return X_VAL_1;
			} else if (val[0] == X_VAL_X || val[1] == X_VAL_X) {
				return X_VAL_X;
			} else {
				return (val[1] == X_VAL_1) ? X_VAL_1 : X_VAL_0;
			}
		case EQUIV:
			if (val[0] == X_VAL_X || val[1] == X_VAL_X) {
				return X_VAL_X;
			} else {
				return (val[0] == val[1]) ? X_VAL_1 : X_VAL_0;
			}
		default:
			return X_VAL_X;
	}
}

// eval_expr
// evaulates an expression using stored X values
// args: N/A
// returns: 0 if the expression cannot be solved yet, 1 if the value is 0, 2 if the value is 1
x_val_t eval_expr(x_val_t *xvals) {
	return __eval_node(expr.root, xvals);
}