#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parse.h"

const char EXPR_STRS[5][10] = {
	"not(",
	"and(",
	"or(",
	"imp(",
	"equiv(",
};

#define parseErr(format, ...) \
	fprintf(stderr, "\x1B[31m" "Parsing Error: " format "\x1B[0m", ##__VA_ARGS__)

// new_node
// checks if the expression and creates a new node
// args: node to check, function, arg number
// returns: pointer to new node or null if invalid
static ParseNode *new_node(ParseNode *root, char c, int arg) {
	char str[10] = {0,};
	ParseNode *temp;
	Func func;
	switch(c) {
		case 'n':
		case 'N':
			func = NOT;
			break;
		case 'a':
		case 'A':
			func = AND;
			break;
		case 'o':
		case 'O':
			func = OR;
			break;
		case 'i':
		case 'I':
			func = IMP;
			break;
		case 'e':
		case 'E':
			func = EQUIV;
			break;
	}
	str[0] = EXPR_STRS[func][0];
	for (int i = 1; i < strlen(EXPR_STRS[func]); i++) {
		str[i] = getchar();
	}
	if (strncasecmp(EXPR_STRS[func], str, strlen(EXPR_STRS[func]) + 1)) {
		printf("%s, %s\n", EXPR_STRS[func], str);
		return NULL;
	} else {
		if (root->func == ROOT) {
			temp = root;
		} else {
			temp = calloc(1, sizeof(*temp));
			root->val[arg] = (void *)temp;
			root->valType[arg] = FUNC;
		}
		temp->func = func;
		return temp;
	}
}

// parse_expr
// parses an expression
// args: ParseNode tree node to modify, argument (0 or 1)
// returns: 1 if valid expression, 0 if not
int parse_expr(ParseNode *node, int arg) {
	ParseNode *temp;
	int c;
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
				if (!(temp = new_node(node, c, arg))) {
					parseErr("Invalid function\n");
					return 0;
				}
				if (!parse_expr(temp, 0)) {
					return 0;
				}
				break;
			case 'x':
			case 'X':
				xval = 0;
				c = getchar();
				if (c < '0' || c > '9') {
					parseErr("Invalid X Value\n");
					return 0;
				}
				while (c >= '0' && c <= '9') {
					xval *= 10;
					xval += c - '0';
					c = getchar();
				}
				ungetc(c, stdin);
				node->valType[arg] = X_VAL;
				node->val[arg] = (void *) xval;
				return 1;
			case '0':
			case '1':
				node->valType[arg] = C_VAL;
				node->val[arg] = (void *) (long)(c - '0');
				return 1;
			default:
				parseErr("Invalid Expression:%c\n", c);
				return 0;
		}
		c = getchar();
		while (c == ' ' || c == '\t') {
			c = getchar();
		}
		if (temp->func == NOT) {
			if (c != ')') {
				parseErr("Missing Parenthesis\n");
				return 0;
			} else {
				return 1;
			}
		} else {
			if (c != ',') {
				parseErr("Missing Comma\n");
				return 0;
			}
		}
		if (!(parse_expr(temp, 1))) {
			return 0;
		}
		c = getchar();
		while (c == ' ' || c == '\t') {
			c = getchar();
		}
		if (c != ')') {
			parseErr("Missing Parenthesis\n");
			return 0;
		} else {
			return 1;
		}
	}
	parseErr("Escaped the while loop somehow\n");
	return 0;
}

// print_expr
// prints an expression
// args: ParseNode tree node to print
// returns: N/A
void print_expr(ParseNode *root) {
	int numArgs = (root->func == NOT) ? 1 : 2;
	printf("( ");
	for (int i = 0; i < numArgs; i++) {
		if (i == numArgs - 1) {
			switch(root->func) {
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
		switch(root->valType[i]) {
			case FUNC:
				print_expr((ParseNode *) root->val[i]);
				break;
			case X_VAL:
				printf("x");
			case C_VAL:
				printf("%ld", (long) root->val[i]);
				break;
			case NONE:
				printf(" ERROR in valType value ");
				break;
		}
	}
	printf(" )");
}

// del_expr
// deletes an expression
// args: ParseNode tree node to delete
// returns: N/A
void del_expr(ParseNode *root) {
	for (int i = 0; i < 2; i++) {
		if (root->valType[i] == FUNC) {
			del_expr(root->val[i]);
		}
		root->valType[i] = NONE;
	}
	if (root->func != ROOT) {
		free(root);
	}
	return;
}