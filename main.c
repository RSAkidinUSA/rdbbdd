#include <stdio.h>
#include "parse.h"
#include "bdd.h"

// Main function
// args: # args to program, args to program
// returns: 0 on success, other value on failure
int main(int argc, char **argv) {
	int retval;
	ParseNode temp;
	temp.func = ROOT;
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
		if ((retval = parse_expr(&temp, 0))) {
			break;
		} else {
			/* print expression */
			print_expr(&temp);
			printf("\n");
			/* build robdd */
			init_h_table(100);
			init_t_table(100);
			/* delete robdd */
			free_h_table();
			free_t_table();
			/* delete expression */
			temp.func = ROOT;
			del_expr(&temp);
		}
	} while(1);
	printf("Error encountered when parsing expression, exiting now\n");
	return retval;
}
