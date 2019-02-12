/* mk_table_t
   table of value, low and high of MK tables T and H
*/

/* init and free functions */
void init_h_table(int size);
void free_h_table();

void init_t_table(int size);
void free_t_table();

/* other functions */
int MK(int i, int l, int h);