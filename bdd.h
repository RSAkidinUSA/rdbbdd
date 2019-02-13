#define NUM_TABLE_ENTRIES 1000000 // 1 million table entries

/* init and free functions */
void init_t_table(int size);
void free_t_table(void);

void init_h_table(void);
void free_h_table(void);

/* other functions */
int MK(int i, int l, int h);
int BUILD(int t);
void printMK(void);