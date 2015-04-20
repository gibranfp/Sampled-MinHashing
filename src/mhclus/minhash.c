/**
 * @file minhash.c
 * @author Gibran Fuentes Pineda <gibranfp@turing.iimas.unam.mx>
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
#include "minhash.h"

/**
 * @brief Prints head of a hash table structure
 *
 * @param hash_table Hash table structure
 */
void mh_print_head(HashTable *hash_table)
{
	uint i;

	printf("========== Hash table =========\n");
	printf("Table size: %d\n"
		   "Tuple size: %d\n"
		   "Dimensionality: %d\n"
		   "Used buckets: ",
		   hash_table->table_size, 
		   hash_table->tuple_size,
		   hash_table->dim); 
	list_print(&hash_table->used_buckets);

	printf("a: ");
	for (i = 0; i < hash_table->tuple_size; i++)
		printf("%d ", hash_table->a[i]);

	printf("\nb: ");
	for (i = 0; i < hash_table->tuple_size; i++)
		printf("%d ", hash_table->b[i]);
	printf("\n");
}

/**
 * @brief Prints head and content of a hash table structure
 *
 * @param hash_table Hash table structure
 */
void mh_print_table(HashTable *hash_table)
{
	uint i;
	for (i = 0; i < hash_table->used_buckets.size; i++){
		printf("[  %d  ] ", hash_table->used_buckets.data[i].item);
		list_print(&hash_table->buckets[hash_table->used_buckets.data[i].item].items);
	}
}

/**
 * @brief Initializes a hash table structure for performing Min-Hash
 *        on a collection of list.
 *
 * @param hash_table Hash table structure
 * @param Number of MinHash values per tuple
 * @param dim Largest item value in the database of lists
 * @param table_size Number of buckets in the hash table
 */
void mh_init(HashTable *hash_table)
{
	hash_table->table_size = 0;
	hash_table->tuple_size = 0; 
	hash_table->dim = 0; 
	hash_table->permutations  = NULL; 
	hash_table->buckets = NULL;
	list_init(&hash_table->used_buckets);
	hash_table->a = NULL;
	hash_table->b = NULL;
}

/**
 * @brief Creates a hash table structure for performing Min-Hash
 *        on a collection of list.
 *
 * @param Number of MinHash values per tuple
 * @param dim Largest item value in the database of lists
 * @param table_size Number of buckets in the hash table
 *
 * @return Hash table structure
 */
HashTable mh_create(uint table_size, uint tuple_size, uint dim)
{
	HashTable hash_table;

	hash_table.table_size = table_size;
	hash_table.tuple_size = tuple_size; 
	hash_table.dim = dim; 
	hash_table.permutations  = (RandomValue *) malloc(tuple_size * dim * sizeof(RandomValue)); 

	hash_table.buckets = (Bucket *) calloc(table_size, sizeof(Bucket));
	list_init(&hash_table.used_buckets);

	uint i;
	hash_table.a =  (uint *) malloc(tuple_size * sizeof(uint));
	hash_table.b = (uint *) malloc(tuple_size * sizeof(uint));
	for (i = 0; i < tuple_size; i++){
		hash_table.a[i] = rand();
		hash_table.b[i] = rand();
	}

	return hash_table;
}

/**
 * @brief Removes items stored in a bucket whose index is computed from a given list
 *
 * @param list List to be removed
 * @param hash_table Hash table structure
 */
void mh_erase_from_list(List *list, HashTable *hash_table)
{	
	uint index = mh_get_index(list, hash_table);
	list_destroy(&hash_table->buckets[index].items);
	hash_table->buckets[index].hash_value = 0;
	Item item = {index, 1};
	Item *ptr = list_find(&hash_table->used_buckets, item);
	uint position = (uint) (ptr - hash_table->used_buckets.data);
	list_delete_position(&hash_table->used_buckets, position);
}

/**
 * @brief Removes items in a given bucket of the hash table
 *
 * @param index Index of the bucket to be removed
 * @param hash_table Hash table structure
 */
void mh_erase_from_index(uint index, HashTable *hash_table)
{	
	if (index >= 0 && index < hash_table->table_size){
		list_destroy(&hash_table->buckets[index].items);
		hash_table->buckets[index].hash_value = 0;
		Item item = {index, 1};
		Item *ptr = list_find(&hash_table->used_buckets, item);
		uint position = (uint) (ptr - hash_table->used_buckets.data);
		list_delete_position(&hash_table->used_buckets, position);
	} else 
		printf("Index %u out of range! Table size is %u", index, hash_table->table_size);
}

/**
 * @brief Removes the items in all the used buckets of the hash table
 *
 * @param hash_table Hash table structure
 */
void mh_clear_table(HashTable *hash_table)
{	
	uint i;

	for (i = 0; i < hash_table->used_buckets.size; i++) {
		list_destroy(&hash_table->buckets[hash_table->used_buckets.data[i].item].items);
		hash_table->buckets[hash_table->used_buckets.data[i].item].hash_value = 0;
	}

	list_destroy(&hash_table->used_buckets);
}

/**
 * @brief Destroys a hash table structure 
 *
 * @param hash_table Hash table structure
 */
void mh_destroy(HashTable *hash_table)
{	
	free(hash_table->permutations);
	free(hash_table->buckets);
	free(hash_table->a);
	free(hash_table->b);
	list_destroy(&hash_table->used_buckets);
	mh_init(hash_table);
}

/**
 * @brief Assigns, for each MinHash function, a random positive integer 
 *        and a uniformly distributed U(0,1) number to each possible 
 *        item in the database of lists.
 * 
 * @param dim Largest item value in the database of lists
 * @param tuple_size Number of MinHash values per tuple
 * @param permutations Random positive integers assigned to each possible item 
 * @param uniform Uniformly distributed random numbers U(0,1) assigned to 
 *                each possible item 
 */
void mh_generate_permutations(uint dim, uint tuple_size, RandomValue *permutations)
{
	uint i, j;
	uint rnd;
     
	// generates random permutations by assigning a random value to each item
	// of the universal set
	for (i = 0; i < tuple_size; i++){ 
		for (j = 0; j < dim; j++){
			rnd = (uint) rand();
			permutations[i * dim + j].random_int = rnd;
			permutations[i * dim + j].random_double = -logl((double) rnd / ((double) RAND_MAX + 1));
		}
	}
}

/**
 * @brief Assigns, for each MinHash function, a random positive integer 
 *        and a uniformly distributed U(0,1) number to each possible 
 *        item in the database of lists.
 * 
 * @param list List to be hashed
 * @param pernum Number of the permutation
 * @param permutations Random permutations
 * @param dim Dimensionality of the data (number of lists)
 */
uint mh_compute_minhash(List *list, RandomValue *permutations)
{
	uint i;
	double *values;

	// get randomly assigned values for list
	// and find minimum value
	uint min_int = permutations[list->data[0].item].random_int;
	double min_double = permutations[list->data[0].item].random_double;
	for (i = 1; i < list->size; i++){
		if (min_double > permutations[list->data[i].item].random_double){
			min_int = permutations[list->data[i].item].random_int;
			min_double = permutations[list->data[i].item].random_double;
		}
	}
     
	return min_int;
}

/**
 * @brief Universal hashing for getting a hash table index from the corresponding minhash tuple
 *
 * @param list List to be hashed
 * @param hash_table Hash table structure
 * @param hash_value Hash value
 * @param index Table index
 */
void mh_univhash(List *list, HashTable *hash_table, uint *hash_value, uint *index)
{
	uint i, minhash;
	unsigned long long temp_index = 0;
	unsigned long long temp_hv = 0;

	for (i = 0; i < hash_table->tuple_size; i++){ // computes MinHash values
		minhash = mh_compute_minhash(list, hash_table->permutations + i * hash_table->dim);
		temp_index += hash_table->a[i] * minhash;
		temp_hv += hash_table->b[i] * minhash; 
	}

	// computes 2nd-level hash value and index (universal hash functions)
	*hash_value = (temp_hv % LARGEST_PRIME);	
	*index = (temp_index % LARGEST_PRIME) % hash_table->table_size;
}

/**
 * @brief Computes 2nd-level hash value of lists using open 
 *        adressing collision resolution and linear probing.
 * @todo Add other probing strategies.
 *
 * @param list List to be hashed
 * @param hash_table Hash table structure
 *
 * @return - index of the hash table
 */ 
uint mh_get_index(List *list, HashTable *hash_table)
{
	uint checked_buckets, index, hash_value;
     
	mh_univhash(list, hash_table, &hash_value, &index);
	if (hash_table->buckets[index].items.size != 0){ // examine buckets (open adressing)
		if (hash_table->buckets[index].hash_value != hash_value){
			checked_buckets = 1;
			while (checked_buckets < hash_table->table_size){ // linear probing
				/* index = (index + 1) % hash_table->table_size; */
				index = ((index + 1) & (hash_table->table_size - 1));
				if (hash_table->buckets[index].items.size != 0){
					if (hash_table->buckets[index].hash_value == hash_value)
						break;	
				}
				else{
					hash_table->buckets[index].hash_value = hash_value; 
					break;
				}
				checked_buckets++;	     
			}
			if (checked_buckets == hash_table->table_size){
				fprintf(stderr,"Error: The hash table is full!\n ");
				exit(EXIT_FAILURE);
			}
		}
	}
	else{
		hash_table->buckets[index].hash_value = hash_value; 
	}
     
	return index;
}

/**
 * @brief Stores lists in the hash table.
 *
 * @param list List to be hashed
 * @param id ID of the list
 * @param hash_table Hash table
 */ 
uint mh_store_list(List *list, uint id, HashTable *hash_table)
{
	uint index;
	
	// get index of the hash table
	index = mh_get_index(list, hash_table);

	if (hash_table->buckets[index].items.size == 0){ // mark used bucket
		Item new_used_bucket = {index, 1};
		list_push(&hash_table->used_buckets, new_used_bucket);
	}

	// store list id in the hash table
	Item new_item = {id, 1};
	list_push(&hash_table->buckets[index].items, new_item);

	return index;
}

/**
 * @brief Stores lists in the hash table.
 *
 * @param listdb Database of lists to be hashed
 * @param hash_table Hash table
 * @param indices Indices of the used buckets
 */ 
void mh_store_listdb(ListDB *listdb, HashTable *hash_table, uint *indices)
{
	uint i;	
			
	// hash all lists in the database
	for (i = 0; i < listdb->size; i++)
		if (listdb ->lists[i].size > 0)
			indices[i] = mh_store_list(&listdb->lists[i], i, hash_table);
}
\
