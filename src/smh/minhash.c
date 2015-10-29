/**
 * @file minhash.c
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
 * @brief Functions for computing MinHash functions, creating hash tables
 *        and storing data into them. Hash tables as arrays of single
 *        linked lists and conflict resolution is done with separate
 *        chaining and open adressing.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <inttypes.h>
#include "mt64.h"
#include "ifindex.h"
#include "minhash.h"

/**
 * @Brief Prints head of a hash table structure
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
          printf("%u ", hash_table->a[i]);

     printf("\nb: ");
     for (i = 0; i < hash_table->tuple_size; i++)
          printf("%u ", hash_table->b[i]);
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
 * @brief Initializes the randon number generator
 */
void mh_rng_init(void)
{
     unsigned long long init[4]={0x12345ULL, 0x23456ULL, 0x34567ULL, 0x45678ULL}, length=4;
     init_by_array64(init, length);
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
     uint i;
     HashTable hash_table;

     hash_table.table_size = table_size;
     hash_table.tuple_size = tuple_size; 
     hash_table.dim = dim; 
     hash_table.permutations = (RandomValue *) malloc(tuple_size * dim * sizeof(RandomValue)); 
    
     hash_table.buckets = (Bucket *) calloc(table_size, sizeof(Bucket));
     list_init(&hash_table.used_buckets);

     // generates array of random values for universal hashing
     hash_table.a = (uint *) malloc(tuple_size * sizeof(uint));
     hash_table.b = (uint *) malloc(tuple_size * sizeof(uint));
     for (i = 0; i < tuple_size; i++){
          hash_table.a[i] = (unsigned int) (genrand64_int64() & 0xFFFFFFFF);
          hash_table.b[i] = (unsigned int) (genrand64_int64() & 0xFFFFFFFF);
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
          // destroy bucket
          list_destroy(&hash_table->buckets[index].items);
          hash_table->buckets[index].hash_value = 0;

          // delete bucket index from list of used buckets
          Item item = {index, 1};
          Item *ptr = list_find(&hash_table->used_buckets, item);
          uint position = (uint) (ptr - hash_table->used_buckets.data);
          list_delete_position(&hash_table->used_buckets, position);
     } else {
          printf("Index %u out of range! Table size is %u", index, hash_table->table_size);
     }
}

/**
 * @brief Removes the items in all the used buckets of the hash table
 *
 * @param hash_table Hash table structure
 */
void mh_clear_table(HashTable *hash_table)
{  
     uint i;

     // empties used buckets of a hash table
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
 */
void mh_generate_permutations(uint dim, uint tuple_size, RandomValue *permutations)
{
     uint i, j;
     ullong rnd;

     // generates random permutations by assigning a random value to each item
     // of the universal set
     for (i = 0; i < tuple_size; i++){ 
          for (j = 0; j < dim; j++){
               rnd = genrand64_int64();
               permutations[i * dim + j].random_int = rnd;
               permutations[i * dim + j].random_double = -logl((rnd >> 11) * (1.0/9007199254740991.0));
          }
     }
}

/**
 * @brief Weights the random numbers assigned to each item
 * 
 * @param dim Largest item value in the database of lists
 * @param tuple_size Number of MinHash values per tuple
 * @param permutations Random positive integers assigned to each possible item
 * @param weights Weight of each item
 */
void mh_weight_permutations(uint dim, uint tuple_size, RandomValue *permutations, double *weights)
{
     uint i, j;

     // weights the assigned random value of each item
     for (i = 0; i < tuple_size; i++)
          for (j = 0; j < dim; j++)
               permutations[i * dim + j].random_double /= weights[j];
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
ullong mh_compute_minhash(List *list, RandomValue *permutations)
{
     uint i;

     // get randomly assigned values for list
     // and find minimum value
     ullong min_int = permutations[list->data[0].item].random_int;
     double min_double = permutations[list->data[0].item].random_double / (double) list->data[0].freq;
     for (i = 1; i < list->size; i++){
          double current_value = permutations[list->data[i].item].random_double / (double) list->data[i].freq;
          if (min_double > current_value){
               min_int = permutations[list->data[i].item].random_int;
               min_double = current_value;
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
     uint i;
     ullong minhash;
     __uint128_t temp_index = 0;
     __uint128_t temp_hv = 0;

     // computes MinHash values
     for (i = 0; i < hash_table->tuple_size; i++){
          minhash = mh_compute_minhash(list, &hash_table->permutations[i * hash_table->dim]);
          temp_index += ((ullong) hash_table->a[i]) * minhash;
          temp_hv += ((ullong) hash_table->b[i]) * minhash; 
     }

     // computes 2nd-level hash value and index (universal hash functions)
     *hash_value = (temp_hv % LARGEST_PRIME64);   
     *index = (temp_index % LARGEST_PRIME64) % hash_table->table_size;
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
                    index = ((index + 1) & (hash_table->table_size - 1));
                    if (hash_table->buckets[index].items.size != 0){
                         if (hash_table->buckets[index].hash_value == hash_value)
                              break;   
                    } else {
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
     } else {
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

/**
 * @brief Computes the cumulative maximum frequencies of a database of lists
 *
 * @param listdb Database of lists
 * @param ifindex Inverted file index of the database of lists
 *
 * @return Array with the cumulative maximum frequencies
 */ 
uint *mh_get_cumulative_frequency(ListDB *listdb, ListDB *ifindex)
{
     uint i, j, k;

     // computes the cumulative maximum frequency of the inverted index
     uint *maxfreq = calloc(ifindex->size, sizeof(uint));
     Item *maxitem = list_max_freq(&ifindex->lists[0]);
     maxfreq[0] = maxitem->freq;
     for (i = 1; i < ifindex->size; i++) {    
          maxitem = list_max_freq(&ifindex->lists[i]);
          maxfreq[i] = maxfreq[i - 1] + maxitem->freq;
     }

     return maxfreq;
}

/**
 * @brief Generates a database of lists with frequencies equal to 1
 *        from a database of lists with frequencies greater than 1
 *
 * @param listdb Database of lists with frequencies greater than 1
 * @param maxfreq Array with the cumulative maximum frequencies
 *                to generate an expanded set of items
 *
 * @return Expanded database of lists with frequencies equal to 1
 */ 
ListDB mh_expand_listdb(ListDB *listdb, uint *maxfreq)
{
     uint i, j, k;
     ListDB expldb = listdb_create(listdb->size, maxfreq[listdb->dim - 1] - 1);

     // creates a binary listdb with the cumulative maximum frequency
     for (i = 0; i < listdb->size; i++) {
          for (j = 0; j < listdb->lists[i].size; j++) {
               for (k = 0; k < listdb->lists[i].data[j].freq; k++) {
                    if (listdb->lists[i].data[j].item != 0) {
                         Item item = {maxfreq[listdb->lists[i].data[j].item - 1] + k, 1};
                         list_push(&expldb.lists[i], item);
                    } else {
                         Item item = {k, 1};
                         list_push(&expldb.lists[i], item);
                    }
               }
          }
     }
     
     return expldb;
}

/**
 * @brief Expands an array of weights based on an expanded set of items
 *
 * @param ifindex Inverted file index of the database of lists
 * @param maxfreq Array with the cumulative maximum frequencies
 *                to generate the expanded set of items
 * @param weights Weights of the original set of items
 *
 * @return Array with the weights of the expanded set of items
 */ 
double *mh_expand_weights(uint number_of_items, uint *maxfreq, double *weights)
{
     uint i, j;
     double *new_weights = (double *) malloc(maxfreq[number_of_items - 1] * sizeof(double));

     for (j = 0; j < maxfreq[0]; j++) 
          new_weights[j] = weights[0];
          
     for (i = 1; i < number_of_items; i++) {
          for (j = maxfreq[i - 1]; j < maxfreq[i]; j++) 
               new_weights[j] = weights[i];
     }
     
     return new_weights;
}
