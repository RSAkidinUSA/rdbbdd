#ifndef BDD_H
#define BDD_H

#define NUM_TABLE_ENTRIES 1000000 // 1 million table entries

// init and free functions
void init_bdd(int numXs);
void free_bdd(void);

// other functions
int MK(int i, int l, int h);
int BUILD(void);
void printBDD(void);
void printTTable(void);
void printHTable(void);

#endif