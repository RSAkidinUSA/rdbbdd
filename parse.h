#ifndef PARSE_H
#define PARSE_H

typedef enum {NOT = 0, AND = 1, OR = 2, IMP = 3, EQUIV = 4, ROOT = -1} op_t;
typedef enum {FUNC = 3, X_VAL = 2, C_VAL = 1, NONE = 0} expr_val_t;
typedef enum {X_VAL_X = 0, X_VAL_0 = 1, X_VAL_1 = 2} x_val_t;

typedef struct {
	op_t op;
	expr_val_t valType[2];
	void *val[2];
} parse_node_t;

void init_expr(void);
int parse_expr(void);
void print_expr(void);
void del_expr(void);
int get_expr_size(void);
x_val_t eval_expr(x_val_t *xvals);

#endif