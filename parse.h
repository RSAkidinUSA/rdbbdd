#ifndef PARSE_H
#define PARSE_H

typedef enum {NOT = 0, AND = 1, OR = 2, IMP = 3, EQUIV = 4, ROOT = -1} Func;
typedef enum {FUNC = 3, X_VAL = 2, C_VAL = 1, NONE = 0} ValType;
typedef struct ParseNode {
	Func func;
	ValType valType[2];
	void *val[2];
} ParseNode;

int parse_expr(ParseNode *root, int arg);
void print_expr(ParseNode *root);
void del_expr(ParseNode *root);

#endif