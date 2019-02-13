#include <stdio.h>
#include "parse.h"
#include "bdd.h"

// Main function
// args: # args to program, args to program
// returns: 0 on success, other value on failure
int main(int argc, char **argv) {
	int retval;
	do {
		printf("Enter expression\n");
		char c = getchar();
		while (c == ' ' || c == '\t' || c == '\n') {
			c = getchar();	
		}
		if (c == EOF) {
			printf("EOF encountered, exiting\n");
			return 0;
		}
		ungetc(c, stdin);
		init_expr();
		if ((retval = parse_expr())) {
			break;
		} else {
			// print expression
			print_expr();
			printf("\n");
			// build robdd
			init_bdd(get_expr_size());
			// testing
			BUILD();
			printMK();
			// end testing
			// delete robdd
			free_bdd();
			// delete expression
			del_expr();
		}
	} while(1);
	printf("Error encountered when parsing expression, exiting now\n");
	return retval;
}