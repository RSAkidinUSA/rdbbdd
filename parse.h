#ifndef PARSE_H
#define PARSE_H

typedef enum {NOT, AND, OR, IMP, EQUIV} Func;
typedef struct ParseNode {
	Func func;
	void *val[2];
} ParseNode;

int parse_expr(ParseNode *root);

#endif