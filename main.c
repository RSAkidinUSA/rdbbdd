#include "colors.h"
#include "parse.h"
#include "bdd.h"

// get_op
// gets the operation for an apply algorithm
// args: N/A
// returns: op or -1 if EOF
 op_t get_op(void) {
	op_t op = ROOT;
	char c;
	do {
		printf("Enter the operation (n = NOT, a = AND, o = OR, i = IMP, e = EQUIV)\n");
		do {
			c = getchar();
			if (c == EOF) {
				return -1;
			}
		} while (c == '\n' || c == ' ');
		switch(c) {
			case 'e':
				op++;
			case 'i':
				op++;
			case 'o':
				op++;
			case 'a':
				op++;
			case 'n':
				op++;
			default:
				break;
		}
	} while (op == ROOT);
	return op;
}

// analyze_results
// prints the bdd table if it's not a tautology or contradiction
// args: result to analyze
// returns: N/A
static void analyze_result(int res) {
	if (res == 0) {
		printf(KYEL "Expression is a contradiction\n" KRST);
	} else if (res == 1) {
		printf(KGRN "Expression is a tautology\n" KRST);
	} else {
		printTTable(res);
	}
	fflush(stdout);
}

// read_whitespace
// reads whitespace
// args: N/A
// returns: first non-whitespace char read
static char read_whitespace(void) {
	char c = getchar();
	while (c == ' ' || c == '\t' || c == '\n') {
		c = getchar();	
	}
	return c;
}

// read_expression
// reads whitespace until expression starts, then parses expression
// args: expression to save data
// returns: 1 on EOF, 0 on success, < 0 on error
static int read_expression(expr_t *expr) {
	int retval;
	printf("Enter expression:\n");
	char c = read_whitespace();
	if (c == EOF) {
		return 1;
	}
	ungetc(c, stdin);
	init_expr(expr);
	if ((retval = parse_expr(expr))) {
		del_expr(expr);
		return -retval;
	} else {
		return 0;
	}
}

// cleanup
// frees all expressions and bdds
// args: base of array of expressions, number of expressions to free
// returns: N/A
static void cleanup(expr_t *expr, int count) {
	for (int i = 0; i < count; i++) {
		// delete robdd
		free_bdd();
		// delete expression
		del_expr(&expr[i]);
	}
}

// eof_cleanup
// cleans up stuff if eof is encountered in a menu
// args: array of expressions, number, if allowed
// returns: N/A
static void eof_cleanup(expr_t *expr, int count, bool valid) {
	printf("EOF encountered, exiting\n");
	cleanup(expr, count + 1);
	if (valid) {
		exit(0);
	} else {
		exit(1);
	}
}

// read_val_range
// reads an integer from a range
// args: string to print, range, assumes max is greater than min
// returns: EOF if eof encountered, else value read
static int read_val_range(char *msg, int min, int max) {
	int selected = 0, nitems, range = max - min;
	if (range) {
		do {
			printf("%s (%d-%d): ", msg, min, max);
			nitems = scanf("%d", &selected);
			if (nitems == EOF) {
				return EOF;
			}
		} while (!nitems || selected < min || selected > max);
	}
	return selected;
}

// do_apply
// read in the operator and expressions to apply to, then do it
// args: list of expressions, list of bases, number of bdds
// returns: N/A
static void do_apply(expr_t *expr, int *u, int count) {
	int u1, u2;
	op_t op;
	init_expr(&(expr[count]));
	init_bdd(&(expr[count]));
	op = get_op();
	if (op == -1) {
		eof_cleanup(expr, count, false);
	}
	u1 = read_val_range("Select the first expression:", 0, count - 1);
	if (u1 == EOF) {
		eof_cleanup(expr, count, false);
	}
	do {
		u2 = read_val_range("Select the second expression:", 0, count - 1);
		if (u2 == EOF) {
			eof_cleanup(expr, count, false);
		} else if (u2 == u1) {
			printf("Expressions must be different\n");
		}
	} while (u2 == u1);
	// set numXs to the max of the two expressions
	expr[count].numXs = (expr[u1].numXs > expr[u2].numXs) ? expr[u1].numXs : expr[u2].numXs;
	u[count] = APPLY(op, u[u1], u[u2]);
}

// do_restrict
// read the values to restrict, then do so
// args: list of bases, number of bdds
// returns: N/A
static void do_restrict(expr_t *expr, int *u, int count) {
	int selected = 0, node, val;
	selected = read_val_range("Select bdd to restrict:", 0, count);
	if (selected == EOF) {
		eof_cleanup(expr, count, false);
	}
	node = read_val_range("Select variable to restrict:", 1, expr[selected].numXs);
	if (node == EOF) {
		eof_cleanup(expr, count, false);
	}
	val = read_val_range("Select value to restrict it to:", 0, 1);
	if (val == EOF) {
		eof_cleanup(expr, count, false);
	}
	u[selected] = RESTRICT(u[selected], node, val);
}

// print
// print the requested bdd
// args: list of bases, number of bdds
// returns: N/A
static void do_print(expr_t *expr, int *u, int count) {
	int selected = 0;
	selected = read_val_range("Select the bdd to print:", 0, count);
	if (selected == EOF) {
		eof_cleanup(expr, count, false);
	}
	analyze_result(u[selected]);
}

// do_help
// prints the help menu for interactive mode
// args: N/A
// returns: N/A
#define MAX_NUM_EXPR 3
static void do_help(int count) {
	printf("Commands:\n");
	if (count && count < MAX_NUM_EXPR - 1) {
		printf("a {op,u1,u2} - apply operation op to expr u1 and expr u2\n");
	}
	if (count < MAX_NUM_EXPR - 1) {
		printf("i - insert\n");
	}
	printf("n - new bdd\n");
	printf("p %s- print bdd%s\n", count ? "{#} " : "", count ? " #" : "");
	printf("q - quit\n");
	printf("r {%sjb} - restrict bdd%s, at node j, with value b\n", count ? "#" : "", count ? " #" : "");
	printf("h - help\n");
}

// Main function
// args: # args to program, args to program
// returns: 0 on success, other value on failure
int main(int argc, char **argv) {
	int retval, u[MAX_NUM_EXPR];
	char c;
	bool interactive = false, menuDone;
	if (argc > 1) {
		if (!strncasecmp(argv[1], "-i", 2)) {
			interactive = true;
		} else if ((!strncasecmp(argv[1], "-h", 2)) || (!strncasecmp(argv[1], "--help", 2))) {
			printf("Usage: %s [-i]\n", argv[0]);
			printf("Use the -i flag to enable interactive mode\n");
		}
	}
	int count = 0;
	expr_t expr[MAX_NUM_EXPR];
	do {
		if (!(retval = read_expression(&expr[count]))) {
			print_expr(&expr[count]);
			printf("\n");
			// build robdd
			init_bdd(&expr[count]);
			u[count] = BUILD(&expr[count]);
			if (interactive) {
				do {
					menuDone = false;
					printf("Enter a command (h for help): ");
					c = read_whitespace();
					if (c == EOF) {
						eof_cleanup(expr, count, true);
					}
					switch(c) {
						case 'a':
							if (count == 0) {
								printf("Need at least 2 expressions to apply\n");
							} else if (count < MAX_NUM_EXPR - 1) {
								count++;
								do_apply(expr, u, count);
							} else {
								printf("No room to apply any more expressions\n");
							}
							continue;
						case 'i':
							if (count < MAX_NUM_EXPR - 1) {
								count++;
								menuDone = true;
							} else {
								printf("No room to insert any more expressions\n");
							}
							continue;
						case 'n':
							cleanup(expr, count + 1);
							count = 0;
							menuDone = true;
							continue;
						case 'r':
							do_restrict(expr, u, count);
							continue;
						case 'p':
							do_print(expr, u, count);
							continue;
						case 'q':
							cleanup(expr, count + 1);
							return 0;
						case 'h':
						default:
							do_help(count);
							continue;
					}
				} while (!menuDone);
			} else {
				cleanup(expr, count + 1);
			}
		} else {
			if (retval < 0) {
				printf("Error encountered when parsing expression, exiting now\n");
				return retval;
			} else {
				printf("EOF encountered, exiting\n");
				return 0;
			}
		}
	} while(1);
	
	return -1;
}