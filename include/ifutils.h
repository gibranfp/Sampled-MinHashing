/**
 * @brief Structures for handling sets and inverted file structures.
 * 
 * @authors Gibran Fuentes Pineda <gibranfp@turing.iimas.unam.mx>
 *          Ivan Vladimir Meza Ruiz <ivanvladimir@turing.iimas.unam.mx>
 * @date Febrary 2013  
 */
#ifndef IFUTILS_H
#define IFUTILS_H

#include <inttypes.h>
#include "types.h"

typedef struct Node{
     uint item;
     struct Node *next;
}Node;

typedef struct Set{
     Node *head;
     Node *tail;
     uint card;
}Set;

/************************ Function prototypes ************************/
void setdb_write(char *filename, uint **setdb, uint *card, uint n, 
		 uint d);
void setdb_read(char *filename, uint ***setdb,uint **card, uint *n,  
		uint *d);
void setwdb_read(char *filename, uint ***setdb, uint ***weight, 
		 uint **card, uint *n, uint *d);
void setwdb_write(char *filename, uint **setdb, uint **weight, 
		  uint *card, uint n, uint d);
void setdbl_read(char *filename, Set **setdb, uint *n, uint *d);
double jaccard(Set l1, Set l2);
double overlap(Set l1, Set l2);
uint set_inter(Set l1, Set l2);
void set_add(Set *l1, Set *l2);

void setdb_prune( uint **setdb, uint  *setdb_card, uint setdb_num,
		  uint setdb_dim, uint **ifs, uint *ifs_card, 
		  uint ifs_num, uint ifs_dim, uint ***pruned, 
		  uint  **pruned_card, uint *pruned_num, uint stop, 
		  uint dochits, double ovr_th, double coocc_th);
void query_ifs(uint *query, uint query_card, uint **ifs, uint *ifs_card, 
	       uint *docovr, uint **retdocs, uint *retdocs_card, 
	       double ovr_th);
void setdb_reassign(uint *inset, uint inset_card, uint **outset, 
		    uint *outset_card, uint **ifs, uint *ifs_card, 
		    uint *docovr, uint *retdocs, uint retdocs_card,
		    double coocc_th);
#endif
