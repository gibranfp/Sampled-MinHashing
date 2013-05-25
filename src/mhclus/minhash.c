/**
 * @file minhash.c
 * @authors Gibran Fuentes Pineda <gibranfp@turing.iimas.unam.mx>
 *          Ivan Vladimir Meza Ruiz <ivanvladimir@turing.iimas.unam.mx>
 * @version 0.1
 * @date 2013
 *
 * @section GPL
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @brief Functions for computing MinHash functions, creating hash tables
 *        and storing data into them. Hash tables as arrays of single 
 *        linked lists and conflict resolution is done with separate
 *        chaining and open adressing.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "types.h"
#include "ifutils.h"
#include "minhash.h"


/**
 * @brief Creates and initializes a structured, r, l, for performing MinHash
 *        on collections of binary sets.
 *
 * @param d Largest item value in the database of sets
 * @param r Number of MinHash values per set
 * @param hmat MinHash values
 * @param htable Hash table
 * @param table_size Number of buckets in the hash table
 * @param a Array of random number for performing 2nd-level hashing
 * @param b Array of random number for performing 2nd-level hashing
 */
void mh_init(uint d, uint r, uint l, uint **hmat, Bucket **htable, 
	     uint table_size, uint **a, uint **b)
{
     uint i;
     uint *ta, *tb;
     
     srand((uint) time(NULL));
     ta = (uint *) malloc(r * sizeof(uint));
     tb = (uint *) malloc(r * sizeof(uint));
     for (i = 0; i < r; i++){// Random values for 2nd-level hashing 
          ta[i] = rand();
	  while (ta[i] == 0)
	       ta[i] = rand();
	  tb[i] = rand();
	  while (tb[i] == 0)
	       tb[i] = rand();
     }
     *a = ta;
     *b = tb;
     *hmat = (uint *) malloc(r * d * sizeof(uint));
     *htable = (Bucket *) calloc(table_size, sizeof(Bucket));
}

/**
 * @brief Assigns, for each MinHash function, a random positive integer 
 *        to each possible item in the database of binary sets.
 * 
 * @param d Largest item value in the database of sets
 * @param r Number of MinHash values per item
 * @param hmat Random positive integers assigned to each possible item 
 */
void mh_randperm(uint d, uint r, uint *hmat)
{
     uint i, j;

     for (i = 0; i < r; i++){//generates random permutations of the items
	  for (j = 0; j < d; j++){//assigns random number to each item
	       hmat[i * d + j] = ((uint) rand() % INF);
	       while (hmat[i * d + j] == 0)
		    hmat[i * d + j] = ((uint) rand() % INF);
	  }
     }
}

/**
 * @brief Computes 2nd-level hash values of binary sets using open 
 *        adressing collision resolution and linear probing.
 * @todo Add other probing strategies.
 *
 * @param set Set to be hashed
 * @param card Cardinality of the set
 * @param hmat Random positive integers assigned to each possible item 
 * @param r Number of MinHash values
 * @param htable Hash table
 * @param table_size Number of buckets in the hash table
 * @param a Random values for 2nd-level hashing
 * @param b Random values for 2nd-level hashing
 *
 * @return - index of the hash table
 */ 
uint mh_getindex(uint *set, uint card, uint d, uint *hmat, uint r, 
		 Bucket *htable, uint table_size, uint *a, uint *b)
{
     uint i, j, k;
     uint index;
     uint hvalue;
     uint minperm;
     double minhv;
     unsigned long long tmp_index, tmp_hv;
     
     tmp_index = 0;
     tmp_hv = 0;  
     for (i = 0; i < r; i++){//computes 2nd-level hashing
	  minhv = INF;
	  //computes the ith MinHash value of the given set
	  for (j = 0; j < card; j++){
	       if (hmat[i * d + set[j]] < minhv){
		    minperm = set[j];
		    minhv = hmat[i * d + set[j]];
	       }
	  }
	  tmp_index += a[i] * hmat[i * d + minperm];
	  tmp_hv += b[i] * hmat[i * d + minperm]; 
     }
     //computes 2nd-level hash value and index (universal hash functions)
     hvalue = (tmp_hv % PRIME);	
     index = (tmp_index % PRIME) % table_size;
     if (htable[index].head != NULL){//examines buckets (open adressing)
	  if (htable[index].hvalue != hvalue){
	       k = 1;
	       while (k < table_size){//linear probing 
		    index = ((index + 1) & (table_size - 1));
		    if (htable[index].head != NULL){
			 if (htable[index].hvalue == hvalue)
			      break;	
		    }
		    else{
			 htable[index].hvalue = hvalue; 
			 break;
		    }
		    k++;	     
	       }
	       if (k == table_size){
		    fprintf(stderr,"Error: The hash table is full!\n ");
		    exit(EXIT_FAILURE);
	       }
	  }
     }
     else{
	  htable[index].hvalue = hvalue; 
     }
     
     return index;
}

/**
 * @brief Stores binary sets in the hash table
 *
 * @param set Set to be hashed
 * @param card Cardinality of the set
 * @param id ID of the set
 * @param d Largest item in the database of binary sets
 * @param hmat Random positive integers assigned to each possible item 
 * @param r Number of MinHash values per tuple
 * @param l Number of MinHash tuples
 * @param htable Hash table
 * @param table_size Number of buckets in the hash table
 * @param a Random values for 2nd-level hashing
 * @param b Random values for 2nd-level hashing
 */ 
void mh_hashset(uint *set, uint card, uint setid, uint d, uint *hmat, 
		uint r, Bucket *htable, uint table_size, uint *a, 
		uint *b)
{
     uint index;
     Node *tmp;

     index = mh_getindex(set, card, d, hmat, r, htable, table_size, 
			 a, b);
     tmp = (Node *) malloc(sizeof(Node));
     tmp->item = setid;
     tmp->next = NULL;
     if (htable[index].head == NULL){//if bucket has not been used
	  htable[index].head = tmp;//set becomes the head and tail
	  htable[index].tail = tmp;
	  htable[index].card = 1;
     }
     else{//if bucket has been used
	  htable[index].tail->next = tmp;//set becomes the tail
	  htable[index].tail = tmp;
	  htable[index].card++;
     }
}			


/**
 * @brief Function for mining a database of sets.
 * 
 * @param setdb Database of sets
 * @param card Cardinality of the set
 * @param n Number of sets in the database 
 * @param d Largest number in the database
 * @param r Number of MinHash values per tuple
 * @param l Number of tuples
 * @param table_size Number of buckets in the hash table
 * @param mined Database of mined co-occurring sets
 * @param mined_card Cardinalities of the mined co-occurring sets
 * @param mined_num Number of mined co-occurring sets
 */
void mh_mine(uint **setdb, uint *card, uint n, uint d, uint r, 
	     uint l, uint table_size, uint ***mined, uint **mined_card,
	     uint *mined_num)
{
     uint i, j, k, x;
     Node *ptr, *tmp;
     uint **ms = NULL;//mined sets 
     uint *mc = NULL;//set cardinalities
     Bucket *htable;//Hash table
     uint *hmat;//random permutations
     uint *a, *b;//Random arrays for 2nd-level hashing
     uint total = 0;
     
     //Hashing database & storing candidates
     printf("Hashing sets . . .\n");
     mh_init(d, r, l, &hmat, &htable, table_size, &a, &b);
     for (i = 0; i < l; i++){
	  mh_randperm(d, r, hmat);
	  printf("\tTable %d: ",i);
	  for (j = 0; j < n; j++)
	       mh_hashset(setdb[j], card[j], j, d, hmat, r, htable,
			  table_size, a, b);
	  k = 0;
	  for (j = 0; j < table_size; j++){
	       if (htable[j].card > 2){//Discards mined with few words
		    ms = (uint **) realloc(ms, 
					   (total + 1) * sizeof(uint *));
		    mc = (uint *) realloc(mc, 
					  (total + 1) * sizeof(uint));
		    mc[total] = htable[j].card;
		    ms[total] = (uint *) malloc(mc[total] * sizeof(uint));
		    x = 0;
		    ptr = htable[j].head;
		    while (ptr != NULL){
			 ms[total][x] = ptr->item;
			 x++;
			 tmp = ptr;
			 ptr = ptr->next;
			 free(tmp);
		    }
		    k++;
		    total++;
	       }
	       else{
		    ptr = htable[j].head;
		    while (ptr != NULL){
			 tmp = ptr;
			 ptr = ptr->next;
			 free(tmp);
		    }
	       }
	       htable[j].head = htable[j].tail = NULL;
	       htable[j].card = htable[j].hvalue = 0;
	  }
	  printf("Mined = %d, Total = %d\n", k, total);
     }
     *mined = ms;
     *mined_card = mc;
     *mined_num = total;
}

/**
 * @brief MinHash-based SLINK clustering.
 *
 * @param setdb Database of sets
 * @param n Number of sets in the database
 * @param d Largest number in the database
 * @param r Number of MinHash values per tuple
 * @param l Number of tuples
 * @param table_size Number of buckets in the hash table
 * @param mined Database of mined co-occurring sets
 * @param mined_card Cardinalities of the mined co-occurring sets
 * @param mined_num Number of mined co-occurring sets
 */
void mh_cluster(Set *setdb, uint n, uint d, uint r, uint l, 
		uint table_size, Set **clusters, uint *cluster_num,
		double ovr_th)
{
     int i, j;
     uint *hmat, *a, *b;
     Bucket *htable;
     Set *clus = NULL;
     uint *checked;
     uint *clus_table;
     uint *ind;
     uint clus_num = 0;
     
     printf("Clustering co-occurring sets . . .\n"
	    "\tNumber of sets = %d, Dimensionality = %d, l = %d, r = %d "
	    "sim_thres = %f\n", n, d, l, r, ovr_th);
     checked = (uint *) calloc(n, sizeof(uint));
     clus_table = (uint *) malloc(n * sizeof(uint));
     ind = (uint *) malloc(n * sizeof(uint));
     mh_init(d, r, l, &hmat, &htable, table_size, &a, &b);
     for (i = 0; i < l; i++){// computes each hash table
	  printf("Hash table %u of %u: %u random permutations for %u sets\n", i, l, r, n);
	  mh_randperm(d, r, hmat);
	  for (j = 0; j < n; j++)
	       mhl_hashset(setdb[j], j, d, hmat, r, htable, 
			   table_size, a, b, ind);
	  for (j = 0; j < n; j++){
	       if (checked[j] == 0){// if the set hasn't been
		    //classified a new cluster is formed 
		    clus = (Set *) realloc(clus, (clus_num + 1) * 
					   sizeof(Set));
		    clus[clus_num].card = 0;
		    mh_add_to_cluster(&clus[clus_num], clus_num, j,
				      checked, clus_table);
		    clus_num++;
	       }
	       mh_check_bucket(setdb, clus, j, &htable[ind[j]], checked, 
			       clus_table, ovr_th);
	  }
     }
     *clusters = clus;
     *cluster_num = clus_num;
     
     printf("Done clustering! %d clusters generated\n",clus_num);
}

/**
 * @brief Converts clusters (sets of ids) to sets of items.
 *
 * @param setdb Database of sets
 * @param cluster Clusters given as sets of set ids
 * @param clus_num Number of clusters
 * @param model Converted clusters (sets of items)
 * @param model_num Number of models generated
 */
void mh_make_model(Set *setdb, Set *cluster, uint clus_num, 
		   Set **model, uint *model_num)
{
     uint i;
     Node *ptr;
     Set *mod = NULL;
     uint mod_num = 0;
     
     for (i = 0; i < clus_num; i++){
     	  if (cluster[i].head != NULL){
     	       mod = (Set *) realloc(mod, (mod_num + 1) * 
				     sizeof(Set));
     	       mod[mod_num].head = mod[mod_num].tail = NULL;
     	       mod[mod_num].card = 0;
     	       ptr = cluster[i].head;
     	       while (ptr != NULL){
		    set_add(&mod[mod_num], &setdb[ptr->item]);
     		    ptr = ptr->next;
	       }
	       mod_num++;
	  }
     }
     *model = mod;
     *model_num = mod_num;
     
     printf("Built %d models\n", mod_num);
}

/**
 * @brief Adds item to cluster.
 *
 * @param cluster Cluster to which a set will be added
 * @param clusid ID of the cluster
 * @param Setid ID of the set to be added
 * @param checked Keeps track of the already checked sets
 * @param clus_table Keeps track of the cluster to which each set is 
 *                   assigned
 */
void mh_add_to_cluster(Set *cluster, uint clusid, uint setid, 
		       uint *checked, uint *clus_table)
{
     Node *tmp;

     tmp = (Node *) malloc(sizeof(Node));
     tmp->item = setid;
     tmp->next = NULL;
     if (cluster->card == 0)
	  cluster->head = cluster->tail = tmp;
     else{
	  cluster->tail->next = tmp;
	  cluster->tail = tmp;
     }
     cluster->card++;
     clus_table[setid] = clusid;
     checked[setid] = 1;
}

/**
 * @brief Merge one cluster into another.
 *
 * @param cl1 Cluster 1
 * @param cl2 Cluster 2
 * @param clusid ID assigned to the resulting merged cluster
 * @param clus_table Keeps track of the cluster to which each set is 
 *                   assigned
 */
void mh_merge_clusters(Set clus1, Set clus2, uint clusid, uint *clus_table)
{
     Node *ptr;

     /* Connecting two clusters */
     clus1.tail->next = clus2.head;
     clus1.tail = clus2.tail;
     clus1.card += clus2.card;
     
     /* The second is deleted*/
     clus2.head = NULL;
     clus2.tail = NULL;
     ptr = clus1.head;
     while(ptr != NULL){
	  clus_table[ptr->item] = clusid;
	  ptr = ptr->next;
     }
}

/**
 * @brief Checks a hash bucket for similar sets to be merged in a 
 *        cluster.
 *
 * @param setdb Database of sets
 * @param clus Generated clusters
 * @param Setid ID of the set to be added
 * @param bucket Hash bucket to be checked
 * @param checked Keeps track of the already checked sets
 * @param clus_table Keeps track of the cluster to which each set is 
 *                   assigned
 */
void mh_check_bucket(Set *setdb, Set *clus, uint setid, Bucket *bucket,
		     uint *checked, uint *clus_table, double ovr_th)
{
     Node *ptr, *prev;

     if (bucket->head != NULL){ // if the bucket has not been checked yet
	  ptr = bucket->head;
	  while (ptr != NULL){
	       if (ptr->item != setid && 
		   overlap(setdb[setid], setdb[ptr->item]) > ovr_th){
		    // cooccurring set is not classified
		    if (checked[ptr->item] == 0)
			 mh_add_to_cluster(&clus[clus_table[setid]], 
					   clus_table[setid], ptr->item, 
					   checked, clus_table);
		    else {// cooccurring set is already classified
			 if (clus_table[ptr->item] > clus_table[setid])
			      mh_merge_clusters(clus[clus_table[setid]],
						clus[clus_table[ptr->item]],
						clus_table[setid], 
						clus_table);
			 else if (clus_table[ptr->item] < clus_table[setid])
			      mh_merge_clusters(clus[clus_table[ptr->item]],
						clus[clus_table[setid]], 
						clus_table[ptr->item], 
						clus_table);
		    }
	       }
	       // Freeing up bucket
	       prev = ptr;
	       ptr = ptr->next;
	       free(prev);
	  }
	  bucket->head = bucket->tail = NULL;
	  bucket->hvalue = 0;
     }
}

/**
 * @brief Creates and initializes a structure for performing MinHash
 *        on collections of weighted sets.
 *
 * @param d Largest item value in the database of sets
 * @param r Number of MinHash values per set
 * @param hmat MinHash values
 * @param htable Hash table
 * @param table_size Number of buckets in the hash table
 * @param a Array of random number for performing 2nd-level hashing
 * @param b Array of random number for performing 2nd-level hashing
 */
void mhw_init(uint d, uint r, uint l, uint **hmat, double **umat, 
	      Bucket **htable, uint table_size, uint **a, uint **b)
{
     uint i;
     uint *ta, *tb;
     
     srand((uint) time(NULL));
     ta = (uint *) malloc(r * sizeof(uint));
     tb = (uint *) malloc(r * sizeof(uint));
     for (i = 0; i < r; i++){//Random values for 2nd-level hashing 
          ta[i] = rand();
	  while (ta[i] == 0)
	       ta[i] = rand();
	  tb[i] = rand();
	  while (tb[i] == 0)
	       tb[i] = rand();
     }
     *a = ta;
     *b = tb;
     *hmat = (uint *) malloc(r * d * sizeof(uint));
     *umat = (double *) malloc(r * d * sizeof(double));
     *htable = (Bucket *) calloc(table_size, sizeof(Bucket));
}

/**
 * @brief Assigns, for each MinHash function, a random positive integer 
 *        and a uniformly distributed U(0,1) number to each possible 
 *        item in the database of weighted sets.
 * 
 * @param d Largest item value in the database of sets
 * @param r Number of MinHash values per item
 * @param hmat Random positive integers assigned to each possible item 
 * @param umat Uniformly distributed random numbers U(0,1) assigned to 
 *             each possible item 
 */
void mhw_randperm(uint d, uint r, uint *hmat, double *umat)
{
     uint i, j;
     uint rnd;
     
     for (i = 0; i < r; i++){//generates random permutations
	  for (j = 0; j < d; j++){//assigns random numbers to each item
	       rnd = (uint) rand();
	       umat[i * d + j] = -log((double)rnd / ((double) RAND_MAX + 1));
	       hmat[i * d + j] = rnd % INF;
	       while (hmat[i * d + j] == 0){
		    rnd = (uint) rand();
		    hmat[i * d + j] = rnd % INF;
	       }
	  }
     }
}

/**
 * @brief Computes 2nd-level hash value of weighted sets using open 
 *        adressing collision resolution and linear probing.
 * @todo Add other probing strategies.
 *
 * @param set Set to be hashed
 * @param weight Weight of each item in the set
 * @param card Numbers of items in the set
 * @param hmat Random positive integers assigned to each possible item
 * @param umat Uniformly distributed random numbers U(0,1) assigned to 
 *               each possible item 
 * @param r Number of MinHash values
 * @param htable Hash table
 * @param table_size Number of buckets in the hash table
 * @param a Random positive integers for 2nd-level hashing
 * @param b Random positive integers for 2nd-level hashing
 *      
 * @return - index of the hash table
 */ 
uint mhw_getindex(uint *set, uint *weight, uint card, uint d,
		  uint *hmat, double *umat, uint r, Bucket *htable,
		  uint table_size, uint *a, uint *b)
{
     int i, j, k;
     uint index;
     uint hvalue;
     uint minperm;
     double minhv;
     unsigned long long tmp_index, tmp_hv;
     
     tmp_index = 0;
     tmp_hv = 0;  
     for (i = 0; i < r; i++){//computes 2nd-level hashing
	  minhv = INF;
	  //computes the ith MinHash value of the given set
	  for (j = 0; j < card; j++){
	       if (umat[i * d + set[j]] / weight[j] < minhv){
		    minperm = set[j];
		    minhv = umat[i * d + set[j]] / weight[j];
	       }
	  }
	  tmp_index += a[i] * hmat[i * d + minperm];
	  tmp_hv += b[i] * hmat[i * d + minperm]; 
     }
     //computes 2nd-level hash value and index (universal hash functions)
     hvalue = (tmp_hv % PRIME);	
     index = (tmp_index % PRIME) % table_size;
     if (htable[index].head != NULL){//examines buckets (open adressing)
	  if (htable[index].hvalue != hvalue){
	       k = 1;
	       while (k < table_size){//linear probing
		    index = ((index + 1) & (table_size - 1));
		    if (htable[index].head != NULL){
			 if (htable[index].hvalue == hvalue)
			      break;	
		    }
		    else{
			 htable[index].hvalue = hvalue; 
			 break;
		    }
		    k++;	     
	       }
	       if (k == table_size){
		    fprintf(stderr,"Error: The hash table is full!\n ");
		    exit(EXIT_FAILURE);
	       }
	  }
     }
     else{
	  htable[index].hvalue = hvalue; 
     }
     
     return index;
}

/**
 * @brief Stores weighted sets in the hash table.
 *
 * @param set Set to be hashed
 * @param weight Weight of each item in the set
 * @param card Cardinality of the set
 * @param id ID of the set
 * @param d Largest item in the database of binary sets
 * @param hmat Random positive integers assigned to each possible item 
 * @param umat Uniformly distributed random numbers U(0,1) assigned to 
 *        each possible item 
 * @param r Number of MinHash values per tuple
 * @param l Number of MinHash tuples
 * @param htable Hash table
 * @param table_size Number of buckets in the hash table
 * @param a Random values for 2nd-level hashing
 * @param b Random values for 2nd-level hashing
 */ 
void mhw_hashset(uint *set, uint *weight, uint card, uint setid, uint d, 
		 uint *hmat, double *umat, uint r, Bucket *htable, 
		 uint table_size, uint *a, uint *b)
{
     uint index;
     Node *tmp;
     
     index = mhw_getindex(set, weight, card, d, hmat, umat, r, 
			  htable, table_size, a, b);
     tmp = (Node *) malloc(sizeof(Node));
     tmp->item = setid;
     tmp->next = NULL;
     if (htable[index].head == NULL){//if bucket has not been used
	  htable[index].head = tmp;//set becomes the head and tail
	  htable[index].tail = tmp;
	  htable[index].card = 1;
     }
     else{//if bucket has been used
	  htable[index].tail->next = tmp;//set becomes the tail
	  htable[index].tail = tmp;
	  htable[index].card++;
     }
}

/**
 * @brief Function for mining a database of binary sets.
 * 
 * @param setdb Database of binary sets
 * @param n Number of sets in the database 
 * @param d Largest number in the database
 * @param r Number of MinHash values per tuple
 * @param l Number of tuples
 * @param table_size Number of buckets in the hash table
 * @param mined Database of mined co-occurring sets
 * @param mined_card Cardinalities of mined co-occurring sets
 * @param table_size Number of buckets in the hash table
 */
void mhw_mine(uint **setdb, uint **weight, uint *card, uint n, uint d,
	      uint r, uint l, uint table_size, uint ***mined, 
	      uint *mined_card, uint *mined_num)
{
     uint i, j, k, x;
     Node *ptr, *tmp;
     uint **ms = NULL;//mined sets 
     uint *mc = NULL;//set cardinalities
     Bucket *htable;//Hash table
     uint *hmat;//Holds random permutations
     double *umat;//Holds random permutations
     unsigned int *a, *b;//Random arrays for 2nd-level hashing
     uint total = 0;

     //Hashing database & storing candidates
     mhw_init(d, r, l, &hmat, &umat, &htable, table_size, &a, &b);  
     for (i = 0; i < l; i++){
	  mhw_randperm(d, r, hmat, umat);
	  printf("\tTable %d: ",i);
	  for (j = 0; j < n; j++)
	       mhw_hashset(setdb[j], weight[j], card[j], j, d, hmat, 
			   umat, r, htable, table_size, a, b);
	  k = 0;
	  for (j = 0; j < table_size; j++){
	       if (htable[j].card > 2){//Discards mined with few words
		    ms = (uint **) realloc(ms, 
					   (total + 1) * sizeof(uint *));
		    mc = (uint *) realloc(mc, 
					  (total + 1) * sizeof(uint));
		    mc[total] = htable[j].card;
		    ms[total] = (uint *) malloc(mc[total] * sizeof(uint));
		    x = 0;
		    ptr = htable[j].head;
		    while (ptr != NULL){
			 ms[total][x] = ptr->item;
			 tmp = ptr;
			 ptr = ptr->next;
			 free(tmp);
		    }
		    k++;
		    total++;
	       }
	       else{
		    ptr = htable[j].head;
		    while (ptr != NULL){
			 tmp = ptr;
			 ptr = ptr->next;
			 free(tmp);
		    }
	       }
	       htable[j].head = htable[j].tail = NULL;
	       htable[j].card = htable[j].hvalue = 0;
	  }
	  printf("Mined = %d, Total = %d\n", k, total);
     }
     *mined = ms;
     *mined_card = mc;
     *mined_num = total;
}

/**
 * @brief Computes 2nd-level hash value of binary sets using open 
 *        adressing collision resolution and linear probing. 
 *        The implementation is based on linked lists.
 *
 * @param set Set to be hashed
 * @param d Largest number in the database
 * @param hmat Random positive integers assigned to each possible item
 * @param r Number of MinHash values
 * @param htable Hash table
 * @param table_size Number of buckets in the hash table
 * @param a Random positive integers for 2nd-level hashing
 * @param b Random positive integers for 2nd-level hashing
 *      
 * @return - index of the hash table
 */ 
uint mhl_getindex(Set set, uint d, uint *hmat, uint r, 
		  Bucket *htable, uint table_size, uint *a, uint *b)
{
     uint i, k;
     Node *ptr;
     uint index;
     uint hvalue;
     uint minhv, minperm;
     unsigned long long tmp_index, tmp_hv;
     
     tmp_index = 0;
     tmp_hv = 0;
     for (i = 0; i < r; i++){ // computes second level hashing
	  minhv = INF;
	  ptr = set.head;
	  // computes the ith min-hash value for the given set
	  while (ptr != NULL){ 
	       if (hmat[i * d + ptr->item] < minhv){
		    minperm = ptr->item;
		    minhv = hmat[i * d + ptr->item];
	       }
	       ptr = ptr->next;
	  }
	  tmp_index += a[i] * hmat[i * d + minperm];
	  tmp_hv += b[i] * hmat[i * d + minperm]; 
     }
     hvalue = (tmp_hv % PRIME);	
     index = (tmp_index % PRIME) % table_size;
     if (htable[index].head != NULL){
	  if (htable[index].hvalue != hvalue){
	       k = 1;
	       while (k < table_size){	     
		    index = ((index + 1) & (table_size - 1));
		    if (htable[index].head != NULL){
			 if (htable[index].hvalue == hvalue)
			      break;	
		    }
		    else{
			 htable[index].hvalue = hvalue; 
			 break;
		    }
		    k++;	     
	       }
	       if (k == table_size){
		    fprintf(stderr,"Error: The hash table is full!\n ");
		    exit(1);
	       }
	  }
     }
     else{
	  htable[index].hvalue = hvalue; 
     }
     
     return index;
}

/**
 * @brief Stores binary sets in the hash table.
 *
 * @param set Set to be hashed
 * @param id ID of the set
 * @param d Largest item in the database of binary sets
 * @param hmat Random positive integers assigned to each possible item 
 * @param r Number of MinHash values per tuple
 * @param htable Hash table
 * @param table_size Number of buckets in the hash table
 * @param a Random values for 2nd-level hashing
 * @param b Random values for 2nd-level hashing
 * @param indx_db Stores indexes of set
 */ 
void mhl_hashset(Set set, uint id, uint d, uint *hmat, uint r, 
		 Bucket *htable, uint table_size, uint *a, uint *b, 
		 uint *indx_db)
{
     uint index;
     Node *tmp;
     
     index = mhl_getindex(set, d, hmat, r, htable, table_size, 
			  a, b);
     tmp = (Node *) malloc(sizeof(Node));
     tmp->item = id;
     tmp->next = NULL;
     if (htable[index].head == NULL){  // if bucket has not been used
	  htable[index].head = tmp;  // set becomes the head and tail
	  htable[index].tail = tmp;
	  htable[index].card = 1;
     }
     else{ // if bucket has been used
	  htable[index].tail->next = tmp;  // set becomes the tail
	  htable[index].tail = tmp;
	  htable[index].card++;
     }
     indx_db[id] = index;
}
