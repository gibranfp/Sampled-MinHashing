#include "sampledmh.h"

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

