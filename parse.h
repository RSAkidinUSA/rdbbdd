#ifndef PARSE_H
#define PARSE_H

typedef enum {NOT = 0, AND = 1, OR = 2, IMP = 3, EQUIV = 4, NONE = -1} Func;
typedef enum {FUNC, X_VAL, C_VAL} ValType;
typedef struct ParseNode {
	Func func;
	ValType valType[2];
	void *val[2];
} ParseNode;

int parse_expr(ParseNode *root, int arg);

#endif