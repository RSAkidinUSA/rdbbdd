#include <stdio.h>
#include "parse.h"

// Main function
// args: # args to program, args to program
// returns: 0 on success, other value on failure
int main(int argc, char **argv) {
	printf("Enter expression\n");
	ParseNode temp;
	temp.func = NONE;
	while(parse_expr(&temp, 0)) {
		printf("expression complete\n");
		char c = getchar();
		while (c == ' ' || c == '\t' || c == '\n') {
			c = getchar();	
		}
		ungetc(c, stdin);
	}
	return 0;
}
