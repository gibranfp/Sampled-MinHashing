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
 * @brief Prints help in screen.
 */
void usage(void)
{
     printf("usage: mhclus mine [OPTIONS]... [INPUT_FILE] [OUTPUT_FILE]\n"
	    "       mhclus prune [OPTIONS]... [INVERTED_FILE] [INPUT_FILE] [OUTPUT_FILE]\n"
	    "       mhclus cluster [OPTIONS]... [INPUT_FILE] [OUTPUT_FILE]\n"
	    "Mines inverted file structures, prunes and clusters mined sets,"
	    "and writes the output to a file .\n\n"
	    "General options:\n"
	    "   --help\t\tPrints this help\n"
	    "mine options:\n"
	    "   -s, --size[=1048576]\tNumber of buckets in hash"
	    "table (power of 2)\n"
	    "   -r, --hashes[=4]\tNumber of hashes per tuple\n"
	    "   -l, --tuples[=500]\tNumber of tuples\n"
	    "prune options:\n"
	    "   -s, --stop[=3]\tMinimum set size (stop > 0)\n"
	    "   -d, --dochits[=3]\tMinimum document hits per set (dochits > 0)\n"
	    "   -o, --overlap[=0.7]\tOverlap threshold (0 <= overlap <= 1)\n"
	    "   -c, --coocc[=0.7]\tWord cooccurrence in documents " 
	    " containing set (0 <= coocc <= 1)\n"
	    "cluster options:\n"
	    "   -s, --size[=1048576]\tNumber of buckets in hash"
	    "table (power of 2)\n"
	    "   -r, --tnum[=3]\tNumber of hashes per tuple\n"
	    "   -l, --tuples[=255]\tNumber of tuples\n"
	    "   -o, --overlap[=0.7]\tOverlap threshold\n");
}

/**
 * @brief Stores a database of lists for searching
 *
 * @param listdb Database of lists to be hashed
 * @param tuple_size Number of MinHash values per tuple
 * @param number_of_tuples Number of tuples
 * @param table_size Number of buckets in the hash table
 */
HashIndex mhsearch_build(ListDB *listdb, uint number_of_tuples, uint tuple_size, 
						 uint table_size, uint dim)
{
	HashIndex hash_index;
	hash_index.number_of_tables = number_of_tuples;
	hash_index.hash_tables = (HashTable *) malloc(number_of_tuples * sizeof(HashTable));
	uint *indices = (uint *) malloc(listdb->size * sizeof(uint));

	uint i;
	for (i = 0; i < number_of_tuples; i++) {
		hash_index.hash_tables[i] = mh_create(table_size, tuple_size, dim);
		mh_generate_permutations(dim, tuple_size, hash_index.hash_tables[i].permutations);
		mh_store_listdb(listdb, &hash_index.hash_tables[i], &indices);
	}

	return hash_index;
}

/**
 * @brief Stores a database of lists for searching
 *
 * @param list List to search for near neighbors
 * @param hash_index Index of hash_tables for searching with Min-Hashing
 */
List mhsearch_query(List *query, HashIndex *hash_index)
{
	List neighbors;
	list_init(&neighbors);

	uint i;
	for (i = 0; i < hash_index.number_of_tables; i++) {
		uint index = mh_compute_minhash(query, hash_index.hash_tables[i].permutations);
		list_append(&neighbors, &hash_index.hash_tables[i].buckets[index].items);
	}
	
     list_sort_by_item(&neighbors);
     list_unique(&neighbors);
	
	return neighbors;
}

void mhsearch_check(List *query, List *neighbors, ListDB *listdb, double (*func)(List *, List *))
{
	Score *scores = malloc(neighbors->size);

	uint i;
	for (i = 0; i < neighbors->size; i++){
		scores.index = i;
		scores.value = func(query, &listdb->lists[neighbors->data[i].item]);
	}

	qsort(neighbors->data, neighbors->size, sizeof(Score), listdb_score_compare);
	
}

/**
 * @brief Stores a database of lists for searching
 *
 * @param listdb Database of lists to search for near neighbors
 * @param hash_index Index of hash_tables for searching with Min-Hashing
 */
ListDB mhsearch_query_multi(ListDB *queries, HashIndex *hash_index)
{
	ListDB neighbors = listdb_create(queries->size);

	uint i;
	for (i = 0; i < queries->size; i++) 
		neighbors.lists[i] = mhsearch_query(&queries->lists[i], hash_index);
	
	return neighbors;
}

