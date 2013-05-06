/**
 * @file ifs.c
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
 * @brief Utility functions for reading and writing databases of sets. 
 *
 *        Binary sets format: 
 *
 *             largest_item_value dbsizehelm function emacs
 *	       set1cardinality item1 item2 ...
 *	       set2cardinality item1 item2 ...
 *	       ...
 *
 *        Weighted sets format: 
 *
 *             largest_item_value dbsize
 *	       set1cardinality item1 term_freq1 item2 ...
 *	       set2cardinality item1 term_freq1 item2 ...
 *	       ...
 */
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "ifutils.h"

/**
 *
 * @brief Reads corpus in NIPS format:
 *             card term_id1:frequency1 term_id2:frequency2 ...
 *                        ...
 *
 * @param filename File with the corpus
 * @param corpus Corpus
 * @param termfreq Frequency of each term in the document
 * @param doc_card Number of terms in each document
 * @param corpsize Number of documents in the corpus
 */
void read_corpus(char **filename, uint ***corpus, uint ***termfreq,
		 uint **doccard, uint *corpsize)
{
     FILE *fp;
     fp = fopen(filename, "r");
     if (fp == NULL){
	  fprintf(stderr,"Could not open file %s\n", filename);
	  exit(EXIT_FAILURE);
     }
     
     char *line = NULL;
     size_t len = 0;
     ssize_t read;
     uint **c = NULL; // corpus
     uint **tf = NULL;// term frequencies
     uint *dc = NULL;// document cardinalities
     uint csize = 0;// corpus size
     // reading each line of the file
     while ((read = getline(&line, &len, fp)) != -1) {
	  char *token = strtok (line," ");
	  char **doc;
	  uint term = 0;
	  // spliting line in document terms
	  while (token != NULL)
	  {
	       doc[term] = token;
	       printf ("%s\n",token);
	       token = strtok (NULL, " ");
	       term++;
	  }
	  c = (int **) realloc(c, (csize + 1) * sizeof(int *));
	  c[csize] = (int *) malloc((term - 1) * sizeof(int));
	  tf = (int **) realloc(tf, (csize + 1) * sizeof(int *));
	  tf[csize] = (int *) malloc((term - 1) * sizeof(int));
	  dc = (int *) realloc(dc, (csize + 1) * sizeof(int));
	  dc[csize] = atoi(doc[0]);
	  uint i;
	  for (i = 1; i < term; i++){
	       token = strtok (doc[i],":");
	       c[csize][i] = atoi(token);
	       token = strtok (doc[i],":");
	       tf[csize][i] = atoi(token);
	  }
	  csize++;
     }
     free(line);
     fclose(fp);

     *corpus = c;
     *termfreq = tf;
     *doccard = dc;
     *corpsize = csize;
}

/**
 *
 * @brief Reads vocabulary in NIPS format:
 *             term1 = id1 = termfreq1 termdocfreq1
 *                        ...
 *
 * @param filename File with the vocabulary
 * @param vocab Vocabulary
 * @param termod Term ids
 * @param corpfreq Number of times the term appears in the corpus
 * @param docfreq Number of documents where the term appears
 * @param vocsize Number of terms in the vocabulary
 */
void read_vocab(char **filename, char ***vocab, uint **termid, 
		uint **corpfreq, uint **docfreq, uint *vocsize)
{
     FILE *fp;
     fp = fopen(filename, "r");
     if (fp == NULL){
	  fprintf(stderr,"Could not open file %s\n", filename);
	  exit(EXIT_FAILURE);
     }
     
     char *line = NULL;
     size_t len = 0;
     ssize_t read;
     char **v = NULL;
     uint *cf = NULL;
     uint *df = NULL;
     uint *id = NULL;
     int vsize = 0;
     while ((read = getline(&line, &len, fp)) != -1){
	  // term
	  char *token = strtok (line," = ");
	  v = (char **) realloc(v, (vsize + 1) * sizeof(char *));
	  v[vsize] = (char *) malloc(strlen(token) * sizeof(char));
	  v[vsize] = strcpy(v[vsize],token);
	  // term id
	  token = strtok (NULL, " = ");
	  id = (uint *) realloc(id, (vsize + 1) * sizeof(uint));
	  id[vsize] = atoi(token);
	  // term frequency
	  token = strtok (NULL, " = ");
	  char *sf = strtok (token," ");
	  cf[vsize] = atoi(sf);
	  // term document frequency
	  sf = strtok (token," ");
	  df[vsize] = atoi(sf);
	  vsize++;
     }
     free(line);
     fclose(fp);

     *vocab = v;
     *termid = id;
     *corpfreq = cf;
     *docfreq = df;
     *vocsize = vsize;
}

/**
 * @brief Reads a database of binary sets from a file.
 *
 * @param filename File containing the database of sets
 * @param setdb The database of sets (Array of arrays)
 * @param card Set cardinalities
 * @param n Number of sets in the database
 * @param d Largest set item value
 */
void setdb_read(char *filename, uint ***setdb, uint **card, uint *n, 
		uint *d)
{
     int i, j;
     uint **db, *c;
     uint id, dim, num;
     FILE *file;
  
     printf("Reading sets from %s . . . \n", filename);
     if (!(file = fopen(filename,"r"))){
          fprintf(stderr,"Error: Could not open file %s\n", filename);
          exit(EXIT_FAILURE);
     }
     // reads dimensionality and number of sets
     fscanf(file,"%u %u", &dim, &num); 
     db = (uint **) malloc(num * sizeof(uint *));
     c = (uint *) malloc(num * sizeof(uint));
     for (i = 0; i < num; i ++){     // reads sets
          fscanf(file,"%u", &c[i]); // cardinality of ith set
          db[i] = (uint *) malloc(c[i] * sizeof(uint));
          for (j = 0; j < c[i]; j++){ // reads set items
               fscanf(file,"%u", &id);
               db[i][j] = id;
          }
     }
     if (fclose(file)){
          fprintf(stderr,"Error: Could not close file %s\n", filename);
          exit(EXIT_FAILURE);
     }
     
     *setdb = db;     
     *card = c;     
     *d = dim;
     *n = num;
    
     printf("%u sets read, Dimensionality = %u\n", num, dim);
}

/**
 * @brief Writes a database of binary sets in a file.
 *
 * @param filename File where the database will be saved
 * @param setdb The database of sets (Array of arrays)
 * @param card Set cardinalities
 * @param n Number of sets in the database
 * @param d Largest set item value
 */
void setdb_write(char *filename, uint **setdb, uint *card, uint n, 
		 uint d )
{
     uint i, j;
     FILE *file;
     
     printf("Writing %u sets to file %s . . . \n", n, filename);
     if (!(file = fopen(filename,"w"))){
          fprintf(stderr,"Error: Could not create file %s\n", filename);
          exit(EXIT_FAILURE);
     }
     // writes dimensionality and number of sets
     fprintf(file,"%u %u\n", d, n); 
     for (i = 0; i < n; i ++){     // writes sets
	  fprintf(file,"%u ", card[i]);
          for (j = 0; j < card[i]; j++) // writes each item
	       fprintf(file,"%u ", setdb[i][j]);
	  fprintf(file,"\n");
     }
     if (fclose(file)){
          fprintf(stderr,"Error: Could not close file %s\n", filename);
          exit(EXIT_FAILURE);
     }
}

/**
 * @brief Reads database of weighted sets from a file.
 *
 * @param filename File containing the database of sets
 * @param setdb The database of sets (Array of arrays)
 * @param weight weights of set items (Array of arrays)
 * @param card Set cardinalities
 * @param n Number of sets in the database
 * @param d Largest set item value
 */
void setwdb_read(char *filename, uint ***setdb, uint ***weight, 
		 uint **card, uint *n, uint *d)
{
     uint i, j;
     uint **db, **w, *c;
     uint id, dim, num;
     FILE *file;
     
     printf("Reading sets from %s . . . \n", filename);
     if (!(file = fopen(filename,"r"))){
          fprintf(stderr,"Error: Could not open file %s\n", filename);
          exit(EXIT_FAILURE);
     }
     // reads dimensionality and number of sets
     fscanf(file,"%u %u", &dim, &num); 
     db = (uint **) malloc(num * sizeof(uint *));
     w = (uint **) malloc(num * sizeof(uint *));
     c = (uint *) malloc(num * sizeof(uint));
     for (i = 0; i < num; i ++){     // reads sets
          fscanf(file,"%u", &c[i]); // cardinality of ith set
          db[i] = (uint *) malloc(c[i] * sizeof(uint));
	  w[i] = (uint *) malloc(c[i] * sizeof(uint));
	  for (j = 0; j < c[i]; j++){ // reads set items
               fscanf(file,"%u", &db[i][j]);
	       fscanf(file,"%u", &w[i][j]);
          }
     }
     if (fclose(file)){
          fprintf(stderr,"Error: Could not close file %s\n", filename);
          exit(EXIT_FAILURE);
     }
     *setdb = db;     
     *weight = w;
     *card = c;     
     *d = dim;
     *n = num;
    
     printf("%u sets read, Dimensionality = %u\n", num, dim);
}

/**
 * @brief Writes a database of weighted sets in a file.
 *
 * @param filename File where the database will be saved.
 * @param setdb The database of sets (Array of arrays)
 * @param weight weights of set items (Array of arrays)
 * @param card Set cardinalities
 * @param n Number of sets in the database
 * @param d Largest set item value
 */
void setwdb_write(char *filename, uint **setdb, uint **weight, uint *card,
		  uint n, uint d )
{
     uint i, j;
     FILE *file;
     
     printf("Writing sets to %s . . . \n", filename);
     if (!(file = fopen(filename,"w"))){
          fprintf(stderr,"Error: Could not create file %s\n", filename);
          exit(EXIT_FAILURE);
     }
     // writes dimensionality and number of sets
     fprintf(file,"%u %u", d, n); 
     for (i = 0; i < n; i ++){     // writes sets
	  fprintf(file,"%u ", card[i]);
          for (j = 0; j < card[i]; j++){// writes each item
               fprintf(file,"%u ", setdb[i][j]);
	       fprintf(file,"%u ", weight[i][j]);
	  }
	  fprintf(file,"\n");
     }
     if (fclose(file)){
          fprintf(stderr,"Error: Could not close file %s\n", filename);
          exit(EXIT_FAILURE);
     }
}

/**
 * @brief Reads a database of binary sets (linked sets) from a file.
 *
 * @param filename File containing the database of sets
 * @param setdb The database of sets (array of linked sets)
 * @param n Number of sets in the database
 * @param d Largest set item value
 */
void setldb_read(char *filename, Set **setdb, uint *n, uint *d)
{
     uint i, j, id, dim, num;
     Node *tmp;
     Set *s;
     FILE *file;
     
     printf("Reading sets from %s . . . \n", filename);
     if (!(file = fopen(filename,"r"))){
	  fprintf(stderr,"Error: Could not create file %s\n",filename);
	  exit(EXIT_FAILURE);
     }
     fscanf(file,"%d %d", &dim, &num); // number of items in the list
     s = (Set *) malloc(num * sizeof(Set));
     for (j = 0; j < num; j ++){
	  fscanf(file,"%d", &s[j].card); // number of items in the list
	  s[j].head = s[j].tail = NULL;
	  for (i = 0; i < s[j].card; i++){ // reads each item of the list
	       fscanf(file,"%d", &id);
	       tmp = (Node *) malloc(sizeof(Node));
	       tmp->item = id;
	       tmp->next = NULL;
	       if (s[j].head == NULL)
		    s[j].head = s[j].tail = tmp;
	       else{
		    s[j].tail->next = tmp;
		    s[j].tail = tmp;
	       }    
	  }
     }
     if (fclose(file)){
	  fprintf(stderr,"Error: Could not close file %s\n",
		  filename);
	  exit(EXIT_FAILURE);
     }
     *setdb = s;     
     *d = dim;
     *n = num;
}

/**
 * @brief writes a database of binary sets (linked sets) to a file.
 *
 * @param filename File containing the database of sets
 * @param setdb The database of sets (array of linked sets)
 * @param n Number of sets in the database
 * @param d Largest set item value
 */
void setldb_write(char *filename, Set *setdb, int d, int n)
{
     uint i;
     FILE *file;
     Node *ptr;
  
     if (n > 0){
	  if (!(file = fopen(filename,"w"))){
	       fprintf(stderr,"Error: Could not create file %s\n",
		       filename);
	       exit(EXIT_FAILURE);
	  }
	  fprintf(file,"%d %d\n", d, n);
	  for (i = 0; i < n; i++){
	       fprintf(file,"%d ", setdb[i].card);
	       ptr = setdb[i].head;
	       while (ptr != NULL){	
		    fprintf(file,"%d ", ptr->item);
		    ptr = ptr->next;
	       }
	       fprintf(file,"\n");
	  }
	  if (fclose(file)){
	       fprintf(stderr,"Warning: Error while closing file %s\n",
		       filename);
	       exit(EXIT_FAILURE);
	  }
     }
     else
	  printf("Nothing to save!\n");
}

/**
 * @brief Computes the jaccard coefficient of two sets.
 *
 * @param s1 First se
 * @param s2 Second set
 *
 * @returns Jaccard coefficient
 */
double jaccard(Set s1, Set s2)
{
     uint inter;
     uint uni;
     
     inter = set_inter(s1, s2);
     uni = (s1.card + s2.card) - inter;
     if (uni != 0)
	  return inter / uni;
     else 
	  return -1;
}

/**
 * @brief Computes the overlap coefficient of two sets.
 *
 * @param s1 First set
 * @param s2 Second set
 *
 * @returns Overlap coefficient
 */
double overlap(Set s1, Set s2)
{
     uint inter;
     inter = set_inter(s1, s2);
     if (s1.card < s2.card){
	  if (s1.card != 0)
	       return inter / (s1.card);
	  else 
	       return -1;
     }
     else{
	  if (s2.card != 0)
	       return inter / (s2.card);
	  else 
	       return -1;
     }
}

/**
 * @brief Computes the intersection of two ordered linked set.
 *
 * @param s1 First set
 * @param s2 Second set
 *
 * @returns Set intersection
 */
uint set_inter(Set s1, Set s2)
{
     uint inter;
     Node *ptr1, *ptr2;
     
     inter = 0;
     ptr1 = s1.head;
     ptr2 = s2.head;
     while (ptr1 != NULL && ptr2 != NULL){
	  if (ptr1->item == ptr2->item){
	       inter++;
	       ptr1 = ptr1->next;
	       ptr2 = ptr2->next;	    
	  }
	  else{
	       if(ptr2->item > ptr1->item)
		    ptr1 = ptr1->next;
	       else 
		    ptr2 = ptr2->next;	    
	  }
     }
     
     return inter;
}

/**
 * @brief Adds one ordered set to another.
 *
 * @param *s1 First set 
 * @param *s2 Second set 
 *
 */
void set_add(Set *s1, Set *s2)
{
     Node *ptr1, *ptr2, *prev, *tmp;

     ptr1 = prev = s1->head;
     ptr2 = s2->head;
     while (ptr2 != NULL){
	  //set is empty or item of 2nd set is greater than 1st set
	  if (ptr1 == NULL){ 
	       tmp = (Node *) malloc(sizeof(Node));
	       tmp->item = ptr2->item;
	       tmp->next = NULL;
	       if (s1->head == NULL)
		    s1->head = s1->tail = tmp;
	       else{
		    s1->tail->next = tmp;
		    s1->tail = tmp;
	       }
	       s1->card++;
	       ptr2 = ptr2->next;
	  }
          else{
	       //item of 2nd set is smaller than the head of 1st set
	       if (s1->head->item > ptr2->item){ 
		    tmp = (Node *) malloc(sizeof(Node));
		    tmp->item = ptr2->item;
		    tmp->next = s1->head;
		    s1->head = prev = tmp;
		    s1->card++;
		    ptr2 = ptr2->next;
	       }
	       //item of 1st set is smaller or equal to item of 2nd set
	       else if (ptr1->item >= ptr2->item){ 
		    if (ptr1->item > ptr2->item){ // smaller
			 tmp = (Node *) malloc(sizeof(Node));
			 tmp->item = ptr2->item;
			 tmp->next = ptr1;
			 s1->card++;
			 prev->next = tmp;
			 prev = tmp;
		    }		   
		    else{ // equal
			 prev = ptr1;
			 ptr1 = ptr1->next;
		    }
		    ptr2 = ptr2->next;
	       }
	       else{ // item of 1st set is larger than item of 2nd set
		    prev = ptr1;
		    ptr1 = ptr1->next;
	       }
	  }		 
     }  
}

/**
 * @brief Gets all the documents where a correlated word set might 
 *        appear.
 *
 * @param query Query set
 * @param query_card Cardinality of the query set
 * @param ifs Inverted file
 * @param ifs_card Cardinalities of the inverted file
 * @param docovr 
 * @param retdocs 
 * @param retdocs_card 
 * @param ovr_th 
 */
void query_ifs(uint *query, uint query_card, uint **ifs, uint *ifs_card, 
	       uint *docovr, uint **retdocs, uint *retdocs_card, 
	       double ovr_th)
{
     uint i, j, k;
     uint *count, *docs, docnum;
     
     k = 0;
     count = NULL;
     for (i = 0; i < query_card; i++){ // retrieves each list in inverted
	  for (j = 0; j < ifs_card[query[i]]; j++){
	       if (docovr[ifs[query[i]][j]] == 0){
		    count = realloc(count, (k + 1) * sizeof(uint));
		    count[k] = ifs[query[i]][j];
		    k++;
	       }
	       docovr[ifs[query[i]][j]]++;
	  }
     }
     docnum = 0;
     docs = NULL;
     for (i = 0; i < k; i++){
	  if(docovr[count[i]] < query_card * ovr_th)
	       docovr[count[i]] = 0;
	  else{
	       docs = realloc(docs, (docnum + 1) * sizeof(uint));
	       docs[docnum] = count[i];
	       docnum++;
	  }
     }
     *retdocs = docs;
     *retdocs_card = docnum;
     free(count);
}

/**
 * @brief Discards items that are not consistent.
 *
 * @param setdb Correlated word set database.
 * @param ifs Inverted file.
 * @param *list List of images.
 */
void setdb_reassign(uint *inset, uint inset_card, uint **outset, 
		    uint *outset_card, uint **ifs, uint *ifs_card, 
		    uint *docovr, uint *retdocs, uint retdocs_card, 
		    double coocc_th)
{
     uint i, j;
     uint *os, os_card, hits;
     
     os_card = 0;
     os = NULL;
     for (i = 0; i < inset_card; i++){
	  uint item_occ = 0;
	  // finds number of set items 
	  for (j = 0; j < ifs_card[inset[i]]; j++)
	       if (docovr[ifs[inset[i]][j]] != 0)
		    item_occ++;
	  if (item_occ >= retdocs_card * coocc_th){
	       os = (uint *) realloc(os, (os_card + 1) * 
				     sizeof(uint));
	       os[os_card] = inset[i];
	       os_card++;
	  }
     }
     *outset = os;
     *outset_card = os_card;
}

/**
 * @brief Prunes co-occurring visual words.
 *
 * @param cws_db Database of co-occuring word sets.
 * @param **ifs Inverted file structure.
 * @param *list List of images.
 */
void setdb_prune( uint **setdb, uint  *setdb_card, uint setdb_num,
		  uint setdb_dim, uint **ifs, uint *ifs_card, 
		  uint ifs_num, uint ifs_dim, uint ***pruned, 
		  uint  **pruned_card, uint *pruned_num, uint set_stop, 
		  uint dochits, double ovr_th, double coocc_th)
{
     uint i, j; 
     uint **ps = NULL, *pc = NULL;
     uint  *docovr;
     
     docovr = (uint *) calloc(ifs_dim , sizeof(uint));
     uint ps_num = 0;
     for (i = 0; i < setdb_num; i++){
	  uint *retdocs, retdocs_card;
	  query_ifs(setdb[i], setdb_card[i], ifs, ifs_card, docovr, 
		    &retdocs, &retdocs_card, ovr_th);
	  uint *tmp, tmp_card;
	  setdb_reassign(setdb[i], setdb_card[i], &tmp, &tmp_card, 
			 ifs, ifs_card, docovr, retdocs, retdocs_card, 
			 coocc_th);
	  if (tmp_card >= set_stop && retdocs_card >= dochits){
	       ps = (uint **) realloc(ps, (ps_num + 1) * sizeof(uint *));
	       pc = (uint *) realloc(pc, (ps_num + 1) * sizeof(uint));
	       pc[ps_num] = tmp_card;
	       ps[ps_num] = tmp;
	       ps_num++;
	  }
	  else
	       free(tmp);
	  for (j = 0; j < retdocs_card; j++)
	       docovr[retdocs[j]] = 0;
	  free(retdocs);
     }
     *pruned = ps;
     *pruned_card = pc;
     *pruned_num = ps_num;
}
