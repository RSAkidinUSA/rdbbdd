# rdbbdd
Ryan David Burrow's Binary Decision Diagram

A C implementation of H.K. Andersen’s An Introduction to Binary Decision Diagrams 

Building:
Make all will build with debugging flag
Make O[1,2,3] will build with the appropriate optimization flag

Usage:
Launch with ./rdbbdd
User is then prompted to input an expression and this expression is converted to another representation
Valid expressions must start with an operator (not, and, or, imp, equiv), have appropriate parenthesis and arguments, and can contain x followed by an integer or the constants 1 or 0.
After conversion the following operators are used (!, &&, ||, ->, <->)

The program will keep prompting for inputs until an invalid expression is reached or EOF is provided
