#include <strings.h>
#include <stdbool.h>
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
				printf("EOF encountered, exiting\n");
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

void analyze_result(int res, bdd_t *bdd) {
	if (res == 0) {
		printf(KYEL "Expression is a contradiction\n" KRST);
	} else if (res == 1) {
		printf(KGRN "Expression is a tautology\n" KRST);
	} else {
		printTTable(bdd);
	}
}

// Main function
// args: # args to program, args to program
// returns: 0 on success, other value on failure
int main(int argc, char **argv) {
	int retval;
	bool apply = false;
	if ((argc > 1) && (!strncasecmp(argv[1], "-a", 2))) {
		apply = true;
	}
	int count = 0;
	expr_t expr[2], expRes;
	bdd_t bdd[2], bddRes;
	do {
		printf("Enter expression #%d\n", count + 1);
		char c = getchar();
		while (c == ' ' || c == '\t' || c == '\n') {
			c = getchar();	
		}
		if (c == EOF) {
			printf("EOF encountered, exiting\n");
			return 0;
		}
		ungetc(c, stdin);
		init_expr(&expr[count]);
		if ((retval = parse_expr(&expr[count]))) {
			break;
		} else {
			// print expression
			print_expr(&expr[count]);
			printf("\n");
			// build robdd
			init_bdd(&bdd[count], &expr[count]);
			int res = BUILD(&bdd[count]);
			analyze_result(res, &bdd[count]);
			if (apply) {
				if (count == 1) {
					// apply
					op_t op = get_op();
					if (op == -1) {
						return -1;
					}
					expRes.numXs = expr[0].numXs + expr[1].numXs;
					init_bdd(&bddRes, &expRes);
					res = APPLY(&bddRes, op, &(bdd[0]), &(bdd[1]));
					analyze_result(res, &bddRes);

					// free
					for (int i = 0; i < 2; i++) {
						// delete robdd
						free_bdd(&bdd[i]);
						// delete expression
						del_expr(&expr[i]);
						count = 0;
					}
					free_bdd(&bddRes);
				} else {
					count++;
				}
			} else {
				// delete robdd
				free_bdd(&bdd[count]);
				// delete expression
				del_expr(&expr[count]);
			}
		}
	} while(1);
	printf("Error encountered when parsing expression, exiting now\n");
	return retval;
}