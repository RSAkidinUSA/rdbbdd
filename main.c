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
			init_t_table(get_expr_size());
			init_h_table();
			// delete robdd
			free_t_table();
			free_h_table();
			// delete expression
			del_expr();
		}
	} while(1);
	printf("Error encountered when parsing expression, exiting now\n");
	return retval;
}

/*
int main() {
	init_h_table(15);
	init_t_table(15);
	MK(4,1,0);
	MK(4,0,1);
	MK(3,2,3);
	MK(2,4,0);
	MK(2,0,4);
	MK(1,5,6);
	printMK();
	free_t_table();
	free_h_table();
}
*/