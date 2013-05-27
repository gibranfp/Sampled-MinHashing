/**
 * @brief Structures and functions for the Min-Hash-based clustering.
 * 
 * @authors Gibran Fuentes Pineda <gibranfp@turing.iimas.unam.mx>
 *          Ivan Vladimir Meza Ruiz <ivanvladimir@turing.iimas.unam.mx>
 * @date Febrary 2013  
 */
#ifndef MINHASH_H
#define MINHASH_H

#include <inttypes.h>
#include "types.h"
#include "ifutils.h"

typedef struct Bucket{
     uint hvalue;
     int card;
     Node *head;
     Node *tail;
}Bucket;

/************************ Function prototypes ************************/
/* MinHash functions for binary sets */
void mh_matperm(int *mat, uint n, uint m, uint hnum, uint **hmat);
void mh_init(uint d, uint r, uint l, uint **hmat, Bucket **htable, 
	     uint table_size, uint **a, uint **b);
void mh_randperm(uint d, uint r, uint *hmat);
uint mh_getindex(uint *set, uint card, uint d, uint *hmat, uint r,
 		 Bucket *htable, uint table_size, uint *a, 
 		 uint *b);
void mh_hashset(uint *set, uint card, uint setid, uint d, uint *hmat,
		uint r, Bucket *htable, uint table_size, uint *a,
		uint *b);
void mh_mine(uint **setdb, uint *card, uint n, uint d, uint r, 
	     uint l, uint table_size, uint ***mined, uint **mined_card,
	     uint *mined_num);
void mh_cluster(Set *setdb, uint n, uint d, uint r, uint l, 
		uint table_size, Set **clusters, uint *cluster_num,
		double ovr_th);
void mh_make_model(Set *setdb, Set *cluster, uint clus_num, 
		   Set **model, uint *model_num);
void mh_add_to_cluster(Set *cluster, uint clusid, uint setid, 
		       uint *checked, uint *clus_table);
void mh_check_bucket(Set *setdb, Set *cluster, uint setid, Bucket *bucket, 
		     uint *checked, uint *clus_table, double ovr_th);
void mh_merge_clusters(Set cl1, Set cl2, uint clusid, uint *clus_table);

/* MinHash functions for weighted sets */
void mhw_init(uint d, uint r, uint l, uint **hmat, double **umat,
	      Bucket **htable, uint table_size, uint **a, uint **b);
void mhw_randperm(uint d, uint r, uint *hmat, double *umat);
uint mhw_getindex(uint *set, uint *weight, uint card, uint d,
		  uint *hmat, double *umat, uint r, Bucket *htable,
		  uint table_size, uint *a, uint *b);
void mhw_hashset(uint *set, uint *weight, uint card, uint setid, uint d, 
		 uint *hmat, double *umat, uint r, Bucket *htable, 
		 uint table_size, uint *a, uint *b);
void mhw_mine(uint **setdb, uint **weight, uint *card, uint n, uint d,
	      uint r, uint l, uint table_size, uint ***mined, 
	      uint **mined_card, uint *mined_num);
uint mhl_getindex(Set set, uint d, uint *hmat, uint r, 
		  Bucket *htable, uint table_size, uint *a, uint *b);
void mhl_hashset(Set set, uint setid, uint d, uint *hmat, uint r, 
		 Bucket *htable, uint table_size, uint *a, uint *b, 
		 uint *indx_db);
#endif
