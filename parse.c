#include <stdio.h>
#include <string.h>
#include "parse.h"

// parse_expr
// parses an expression
// args: ParseNode tree root to modify
// returns: 1 if valid expression, 0 if not
int parse_expr(ParseNode *root) {
	ParseNode *node;
	int c, arg = 0;
	node = root;
	while (arg < 2) {
		c = getchar();
		switch(c) {
			case 'n':
				printf("not\n");
				//temp = malloc(sizeof(*node));
				//node->val[arg] = temp;
				node->func = NOT;
				// check rest of word
				// become recursive
				// copy for rest
				if (parse_expr(node)) {
					break;
				} else {
					return 0;
				}
			case 'a':
				printf("and\n");
				node->func = AND;

				break;
			case 'o':
				printf("or\n");
				node->func = OR;

				break;
			case 'i':
				printf("imp\n");
				node->func = IMP;

				break;
			case 'e':
				printf("equiv\n");
				node->func = EQUIV;

				break;
			case 'x':
				printf("X var\n");
				break;
			case '0':
				printf("constant 0\n");
				break;
			case '1':
				printf("constant 1\n");
				break;
			default:
				printf("Invalid expression\n");
				return 0;
		}
		if (arg == 0) {
			c = getchar();
			while (c != ' ' && c != '\t') {
				c = getchar();
			}
			if (node->func == NOT) {
				if (c != ')') {
					printf("Missing Parenthesis\n");
					return 0;
				}
			} else {
				if (c != ')') {
					printf("Missing Comma\n");
					return 0;
				} else {
					arg++;
				}
			}
		} else if (arg == 1) {
			c = getchar();
			while (c != ' ' && c != '\t') {
				c = getchar();
			}
			if (c != ')') {
				printf("Missing Comma\n");
				return 0;
			} else {
				return 1;
			}
		}
	}
	printf("Got to arg 2\n");
	return 0;
}