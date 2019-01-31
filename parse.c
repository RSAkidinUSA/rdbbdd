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
static ParseNode *new_node(ParseNode *root, Func func, int arg) {
	char str[10] = {0,};
	ParseNode *temp;
	str[0] = EXPR_STRS[func][0];
	for (int i = 1; i < strlen(EXPR_STRS[func]); i++) {
		str[i] = getchar();
	}
	if (strncasecmp(EXPR_STRS[func], str, strlen(EXPR_STRS[func]) + 1)) {
		printf("%s, %s\n", EXPR_STRS[func], str);
		return NULL;
	} else {
		if (root->func == NONE) {
			temp = root;
		} else {
			temp = malloc(sizeof(*temp));
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
				if (!(temp = new_node(node, NOT, arg))) {
					parseErr("Invalid function\n");
					return 1;
				}
				if (!parse_expr(temp, 0)) {
					return 0;
				}
				break;
			case 'a':
			case 'A':
				if (!(temp = new_node(node, AND, arg))) {
					parseErr("Invalid function\n");
					return 1;
				}
				if (!parse_expr(temp, 0)) {
					return 0;
				}
				break;
			case 'o':
			case 'O':
				if (!(temp = new_node(node, OR, arg))) {
					parseErr("Invalid function\n");
					return 1;
				}
				if (!parse_expr(temp, 0)) {
					return 0;
				}
				break;
			case 'i':
			case 'I':
				if (!(temp = new_node(node, IMP, arg))) {
					parseErr("Invalid function\n");
					return 1;
				}
				if (!parse_expr(temp, 0)) {
					return 0;
				}
				break;
			case 'e':
			case 'E':
				if (!(temp = new_node(node, EQUIV, arg))) {
					parseErr("Invalid function\n");
					return 1;
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
				node->valType[arg] = C_VAL;
				node->val[arg] = (void *) 0;
				return 1;
			case '1':
				node->valType[arg] = C_VAL;
				node->val[arg] = (void *) 1;
				return 1;
			default:
				printf("c: %c, d: %d\n", c, (int) c);
				parseErr("Invalid Expression\n");
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