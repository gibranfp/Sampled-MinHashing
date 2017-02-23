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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ifindex.h"
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
 * @brief Function for mining a database of lists based on Min-Hashing without weighting.
 *
 * @param listdb Database of binary lists
 * @param tuple_size Number of MinHash values per tuple
 * @param number_of_tuples Number of MinHash tuples
 * @param table_size Number of buckets in the hash table
 */
ListDB sampledmh_mine(ListDB *listdb, uint tuple_size, uint number_of_tuples, uint table_size)
{
     HashTable hash_table = mh_create(table_size, tuple_size, listdb->dim);
     uint *indices = (uint *) malloc(listdb->size * sizeof(uint));

     ListDB coitems;
     listdb_init(&coitems);
     coitems.dim = listdb->size;
          
     // Hashing database & storing candidates
     uint i;
     for (i = 0; i < number_of_tuples; i++){
          printf("\rMining table %u/%u: %u random permutations for %u lists",
                 i + 1, number_of_tuples, tuple_size, listdb->size);
          fflush(stdout);

          mh_generate_permutations(listdb->dim, tuple_size, hash_table.permutations);
          mh_store_listdb(listdb, &hash_table, indices);
          sampledmh_get_coitems(&coitems, &hash_table);
     }
     printf("\n");
     mh_destroy(&hash_table);
     free(indices);

     return coitems;
}

/**
 * @brief Function for mining a database of lists based on Min-Hashing without weighting.
 *
 * @param listdb Database of binary lists
 * @param tuple_size Number of MinHash values per tuple
 * @param number_of_tuples Number of MinHash tuples
 * @param table_size Number of buckets in the hash table
 * @param weights Weight of each item
 */
ListDB sampledmh_mine_weighted(ListDB *listdb, uint tuple_size, uint number_of_tuples, uint table_size,
                               double *weights)
{
     HashTable hash_table = mh_create(table_size, tuple_size, listdb->dim);
     uint *indices = (uint *) malloc(listdb->size * sizeof(uint));

     ListDB coitems;
     listdb_init(&coitems);
     coitems.dim = listdb->size;
          
     // Hashing database & storing candidates
     uint i;
     for (i = 0; i < number_of_tuples; i++){
          printf("Mining table %u/%u: %u random permutations for %u lists\r",
                 i + 1, number_of_tuples, tuple_size, listdb->size);

          mh_generate_permutations(listdb->dim, tuple_size, hash_table.permutations);
          mh_weight_permutations(listdb->dim, tuple_size, hash_table.permutations, weights);
          mh_store_listdb(listdb, &hash_table, indices);
          sampledmh_get_coitems(&coitems, &hash_table);
     }

     mh_destroy(&hash_table);
     free(indices);


     return coitems;
}

/**
 * @brief Generates a database of lists with frequencies equal to 1
 *        from a database of lists with frequencies greater than 1
 *
 * @param listdb Database of lists with frequencies greater than 1
 * @param ifindex Inverted file index of the database of lists
 *
 * @return Expanded database of lists with frequencies equal to 1
 */ 
ListDB sampledmh_expand_frequencies(ListDB *listdb, ListDB *ifindex)
{
     uint *maxfreq = mh_get_cumulative_frequency(listdb, ifindex);
     return mh_expand_listdb(listdb, maxfreq);
     
}

 /**
 * @brief Generates a database of lists with frequencies equal to 1
 *        from a database of lists with frequencies greater than 1
 *        and expands a set of weights to the new set of items.
 *
 * @param listdb Database of lists with frequencies greater than 1
 * @param ifindex Inverted file index of the database of lists
 *
 * @return Expanded database of lists with frequencies equal to 1
 */ 
ListDB sampledmh_expand_frequencies_and_weights(ListDB *listdb, ListDB *ifindex, double *oldweights, double *newweights)
{
     uint *maxfreq = mh_get_cumulative_frequency(listdb, ifindex);
     newweights = mh_expand_weights(ifindex->size, maxfreq, oldweights);
     free(oldweights);
     
     return mh_expand_listdb(listdb, maxfreq);    
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
          uint ovr_th = (uint) round((double) mined->lists[i].size * ovr);
          list_delete_less_frequent(&retdocs.lists[i], ovr_th);
         
          uint cooc_th = (uint) round((double) retdocs.lists[i].size * cooc);
          for (j = 0; j < mined->lists[i].size; j++) {
               // removes items from sets which co-occured in very few documents with the rest
               uint curr_item = mined->lists[i].data[j].item;
               if (list_intersection_size(&ifindex->lists[curr_item], &retdocs.lists[i]) < cooc_th) {
                    list_delete_position(&mined->lists[i], j);
               }
          }

          // destroy mined lists that occur in less than a given number of documents
          if (retdocs.lists[i].size < hits) 
               list_destroy(&mined->lists[i]);
     }

     // removes small sets
     listdb_delete_smallest(mined, stop);
}
