#ifndef PARSE_H
#define PARSE_H

typedef enum {NOT = 0, AND = 1, OR = 2, IMP = 3, EQUIV = 4, ROOT = -1} func_t;
typedef enum {FUNC = 3, X_VAL = 2, C_VAL = 1, NONE = 0} expr_val_t;

typedef struct {
	func_t func;
	expr_val_t valType[2];
	void *val[2];
} parse_node_t;

void init_expr(void);
int parse_expr(void);
void print_expr(void);
void del_expr(void);
int get_expr_size(void);

#endif