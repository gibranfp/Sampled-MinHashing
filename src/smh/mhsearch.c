/**
 * @file mhsearch.c 
 * @author Gibran Fuentes Pineda <gibranfp@turing.iimas.unam.mx>
 * @date 2015
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
 * @brief Functions to perform similarity search by Min-Hashing
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <getopt.h>
#include <inttypes.h>
#include "ifindex.h"
#include "sampledmh.h"
#include "mhlink.h"

/**
 * @brief Builds a MinHash index
 *
 * @param listdb Database of lists to be hashed
 * @param number_of_tuples Number of tuples
 * @param tuple_size Number of MinHash values per tuple
 * @param table_size Number of buckets in the hash table 
 */
HashIndex mhsearch_build(ListDB *listdb, uint number_of_tuples, uint tuple_size, uint table_size)
{
     HashIndex hash_index;
     hash_index.number_of_tables = number_of_tuples;
     hash_index.number_of_tables = (HashTable *) malloc(number_of_tuples * sizeof(HashTable));
     uint *indices = (uint *) malloc(listdb->size * sizeof(uint));

     uint i;
     for (i = 0; i < number_of_tuples; i++) {
	  hash_index.hash_tables[i] = mh_create(table_size, tuple_size, listdb->dim);
	  mh_generate_permutations(listdb->dim, tuple_size, hash_index.hash_tables[i].permutations);
	  mh_store_listdb(listdb, &hash_index.hash_tables[i], &indices);
     }

     return hash_index;
}

/**
 * @brief Makes a query from the MinHash structure
 *
 * @param query Query list
 * @param hash_index Index of hash tables
 */
List mhsearch_query(List *query, HashIndex *hash_index)
{
     List neighbors;
     list_init(&neighbors);

     uint i;
     for (i = 0; i < hash_index->number_of_tables; i++) {
	  uint index = mh_compute_minhash(query, hash_index->hash_tables[i].permutations);
	  list_append(&neighbors, &hash_index->hash_tables[i].buckets[index].items);
     }
	
     list_sort_by_item(&neighbors);
     list_unique(&neighbors);
	
     return neighbors;
}

/**
 * @brief Sorts results of queries 
 *
 * @param query Query list
 * @param neighbors ID's of neighbors found by Min-Hashing
 * @param listdb Database of lists
 * @param func Function to compute score of each neighbor found
 */
void mhsearch_sort_custom(List *query, List *neighbors, ListDB *listdb, double (*func)(List *, List *))
{
     Score *scores = malloc(neighbors->size);

     uint i;
     for (i = 0; i < neighbors->size; i++){
	  scores[i].index = i;
	  scores[i].value = func(query, &listdb->lists[neighbors->data[i].item]);
     }

     qsort(neighbors->data, neighbors->size, sizeof(Score), listdb_score_compare);
}

/**
 * @brief Makes multiple queries from the MinHash structure
 *
 * @param queries Database of lists to query
 * @param hash_index Index of hash tables for searching
 *
 * @return Database of lists with the results of the queries
 */
ListDB mhsearch_query_multi(ListDB *queries, HashIndex *hash_index)
{
     ListDB neighbors = listdb_create(queries->size, queries->dim);

     uint i;
     for (i = 0; i < queries->size; i++) 
	  neighbors.lists[i] = mhsearch_query(&queries->lists[i], hash_index);
	
     return neighbors;
}

