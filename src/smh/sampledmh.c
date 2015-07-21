/**
 * @file sampledmh.c
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
 * @brief Functions to perform Sampled Min-Hashing (SMH)
 */
#include <math.h>
#include "smh/ifindex.h"
#include "smh/sampledmh.h"

/**
 * @brief Retrieves the items that were 
 *
 * @param listdb Database of lists to be hashed
 * @param hash_table Hash table
 */ 
void sampledmh_get_coitems(ListDB *coitems, HashTable *hash_table)
{
	uint i;

	for (i = 0; i < hash_table->used_buckets.size; i++){ // scan buckets to find co-occurring items
		listdb_push(coitems, &hash_table->buckets[hash_table->used_buckets.data[i].item].items);
		list_init(&hash_table->buckets[hash_table->used_buckets.data[i].item].items);
		hash_table->buckets[hash_table->used_buckets.data[i].item].hash_value = 0;
	}

	list_destroy(&hash_table->used_buckets);
}


/**
 * @brief Function for mining a database of binary lists.
 *
 * @param listdb Database of binary lists
 * @param tuple_size Number of MinHash values per tuple
 * @param number_of_tuples Number of MinHash tuples
 * @param table_size Number of buckets in the hash table
 */
ListDB sampledmh_mine(ListDB *listdb, uint tuple_size, uint number_of_tuples, uint table_size)
{
	uint i;
	HashTable hash_table = mh_create(table_size, tuple_size, listdb->dim);
	uint *indices = (uint *) malloc(listdb->size * sizeof(uint));

	ListDB coitems;
   
	listdb_init(&coitems);

	// Hashing database & storing candidates
	for (i = 0; i < number_of_tuples; i++){
		mh_generate_permutations(listdb->dim, tuple_size, hash_table.permutations);
		mh_store_listdb(listdb, &hash_table, indices);
		sampledmh_get_coitems(&coitems, &hash_table);
	}

	mh_destroy(&hash_table);
	free(indices);

	return coitems;
}

/**
 * @brief Prunes co-occurring sets.
 *
 * @param ifindex Inverted file index
 * @param mined Co-occurring sets
 * @param stop Minimum number of items in co-occurring sets
 * @param hits Minimum number of hits of co-occurring set
 * @param ovr Minimum overlap between a co-occurring set and a document to retrieve it
 * @param coocc Minimum overlap between list of retrieved documents and item entry in inverted file
 */
void sampledmh_prune(ListDB *ifindex, ListDB *mined, uint stop, uint hits, double ovr, double cooc)
{
	// query inverted file with mined sets
	ListDB retdocs = ifindex_query_multi(ifindex, mined);

	// leaves documents in which at least ovr_th percent of the mined sets occurred
	uint i, j;
	for (i = 0; i < mined->size; i++) {
		uint ovr_th = (uint) ceil(mined->lists[i].size * ovr);
		list_delete_less_frequent(&retdocs.lists[i], ovr_th);

		double cooc_th = retdocs.lists[i].size * cooc;
		for (j = 0; j < mined->lists[i].size; j++){
			// removes items from sets which co-occured in very few documents with the rest
			uint curr_item = mined->lists[i].data[j].item;
			if (list_intersection_size(&ifindex->lists[curr_item], &retdocs.lists[i]) < cooc_th)
				list_delete_position(&mined->lists[i], j);
		}
	}

	// removes sets which occured in very few documents
	for (i = 0; i < mined->size; i++){
		if (retdocs.lists[i].size < hits){
			listdb_delete_position(mined, i);
			i--;
		}
	}

	// removes small sets
	listdb_delete_smallest(mined, stop);
}
