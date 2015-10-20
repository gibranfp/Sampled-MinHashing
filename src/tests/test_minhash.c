#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "listdb.h"
#include "ifindex.h"
#include "minhash.h"

#define red "\033[0;31m"
#define cyan "\033[0;36m"
#define green "\033[0;32m"
#define blue "\033[0;34m"
#define brown "\033[0;33m"
#define magenta "\033[0;35m"
#define none "\033[0m"

#define MAX_LIST_SIZE 10
#define ELEMENT_MAX_VALUE 15

void test_init_create_print(void)
{
     HashTable htable1;
     mh_init(&htable1);
     mh_print_head(&htable1);
     mh_print_table(&htable1);

	
     HashTable htable2 = mh_create(256, 3, 100);
     mh_print_head(&htable2);

     mh_generate_permutations(htable2.dim, htable2.tuple_size, htable2.permutations);

     uint i, j;
     printf("================= Permutations ==================\n");
     printf("Integers\n");
     for (i = 0; i < htable2.tuple_size; i++){
          printf("tuple %d: {",i);
          for (j = 0; j < htable2.dim; j++)
               printf("[%d]%llu ", j,
                      htable2.permutations[i * htable2.dim + j].random_int);
          printf("}\n");
     }

     printf("Double\n");
     for (i = 0; i < htable2.tuple_size; i++){
          printf("tuple %d: {",i);
          for (j = 0; j < htable2.dim; j++){
               printf("[%d]%lf ", j,
                      htable2.permutations[i * htable2.dim + j].random_double);
          }
          printf("}\n");
     }

     mh_destroy(&htable2);
     mh_print_head(&htable2);
     printf("%s", none);

}

void test_hash(void)
{
     HashTable htable = mh_create(256, 3, 12);
     mh_print_head(&htable);

     mh_generate_permutations(htable.dim, htable.tuple_size, htable.permutations);

     uint i, j;
     printf("================= Permutations ==================\n");
     printf("Integers\n");
     for (i = 0; i < htable.tuple_size; i++){
          printf("tuple %d: {",i);
          for (j = 0; j < htable.dim; j++)
               printf("[%d]%llu ", j,
                      htable.permutations[i * htable.dim + j].random_int);
          printf("}\n");
     }

     printf("Double\n");
     for (i = 0; i < htable.tuple_size; i++){
          printf("tuple %d: {",i);
          for (j = 0; j < htable.dim; j++){
               printf("[%d]%lf ", j,
                      htable.permutations[i * htable.dim + j].random_double);
          }
          printf("}\n");
     }

     printf("\nList = ");
     List list = list_random(5,12);
     while (list.size == 0)
          list = list_random(5,12);

     list_unique(&list);
     list_print(&list);
	
     for (i = 0; i < htable.tuple_size; i++) {
          printf("\t");
          for (j = 0; j < list.size; j++)
               printf("Perm[%u][%u]%llu ", i * htable.dim, list.data[j].item, htable.permutations[list.data[j].item].random_int);
          printf("\n\t");

          for (j = 0; j < list.size; j++)
               printf("Perm[%u][%u]%lf ", i * htable.dim, list.data[j].item, htable.permutations[list.data[j].item].random_double);

         
          uint index = mh_compute_minhash(&list, &htable.permutations[i * htable.dim]);
          printf("\n\tindex[%u] = %u\n", i, index);
     }

     uint hash_value, bucket1, bucket2;
     mh_univhash(&list, &htable, &hash_value, &bucket1);
     bucket2 = mh_get_index(&list, &htable);
     printf("\nhash_value = %u Candidate bucket = %u Final bucket = %u Table hash_value = %llu\n", hash_value, bucket1, bucket2, htable.buckets[bucket2].hash_value);
}

void test_store(void)
{
     List list = list_random(5,12);
     while (list.size == 0)
          list = list_random(5,12);

     list_unique(&list);
     printf("List = ");
     list_print(&list);

     HashTable htable = mh_create(256, 3, 12);
     mh_generate_permutations(htable.dim, htable.tuple_size, htable.permutations);
     uint index = mh_store_list(&list, 0, &htable);
     uint i;
     printf("Index = %u\n", index);
     for (i = 0; i < htable.table_size; i++) {
          printf("[ %u ] ", i);
          list_print(&htable.buckets[i].items);
     }

     printf("Removing list ");
     list_print(&list);
     mh_erase_from_list(&list, &htable);
     for (i = 0; i < htable.table_size; i++) {
          printf("[ %u ] ", i);
          list_print(&htable.buckets[i].items);
     }

     ListDB listdb = listdb_random(12, 12, 12);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);
     listdb_print(&listdb);

     uint *indices;
     indices = malloc(listdb.size * sizeof(uint));
     mh_store_listdb(&listdb, &htable, indices);
     for (i = 0; i < htable.used_buckets.size; i++) {
          printf("[ %u ] ", htable.used_buckets.data[i].item);
          list_print(&htable.buckets[htable.used_buckets.data[i].item].items);
     }
	
     mh_print_head(&htable);
     mh_erase_from_list(&listdb.lists[2], &htable);
     for (i = 0; i < htable.used_buckets.size; i++) {
          printf("[ %u ] ", htable.used_buckets.data[i].item);
          list_print(&htable.buckets[htable.used_buckets.data[i].item].items);
     }
     mh_print_head(&htable);
}

void test_minhash_binary(uint number_of_hashes)
{
     ListDB listdb = listdb_random(50,8,20);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);

     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = 1;

     listdb_print(&listdb);
     
     RandomValue *permutations = (RandomValue *) malloc(number_of_hashes * listdb.dim * sizeof(RandomValue));
     ullong *mhdb = calloc(number_of_hashes * listdb.size, sizeof(ullong));
     mh_generate_permutations(listdb.dim, number_of_hashes, permutations);

     for (i = 0; i < listdb.size; i++) {
          for (j = 0; j < number_of_hashes; j++)
               mhdb[i * number_of_hashes + j] = mh_compute_minhash(&listdb.lists[i], &permutations[j * listdb.dim]);
     }
     
          
     double dist[listdb.size][listdb.size];
     uint collisions[listdb.size][listdb.size];

     for (i = 0; i < listdb.size - 1; i++)
          for (j = i + 1; j < listdb.size; j++) {
               dist[i][j] = list_jaccard(&listdb.lists[i], &listdb.lists[j]);

               uint k;
               for (k = 0; k < number_of_hashes; k++)
                    if ( mhdb[i * number_of_hashes + k] == mhdb[j * number_of_hashes + k])
                         collisions[i][j]++;
          }
     
     for (i = 0; i < listdb.size - 1; i++)
          for (j = i + 1; j < listdb.size; j++)
               printf("Pair (%d, %d): sim = %lf, collisions = %u avg collisions = %lf\n",
                      i, j, dist[i][j], collisions[i][j],
                      (double) collisions[i][j] /  (double) number_of_hashes);
}

void test_minhash_frequency_expanded(uint number_of_hashes)
{
     ListDB listdb = listdb_random(50,8,20);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);
               
     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = rand() % 10 + 1;
     printf("================= Database ==================\n");
     listdb_print(&listdb);

     ListDB ifindex = ifindex_make_from_corpus(&listdb);
     uint *maxfreq = mh_get_cumulative_frequency(&listdb, &ifindex);
     ListDB newdb = mh_expand_listdb(&listdb, maxfreq);
     free(maxfreq);

     RandomValue *permutations = (RandomValue *) malloc(number_of_hashes * newdb.dim * sizeof(RandomValue));
     mh_generate_permutations(newdb.dim, number_of_hashes, permutations);

     ullong *mhdb = (ullong *) calloc(number_of_hashes * listdb.size, sizeof(ullong));
     for (i = 0; i < listdb.size; i++) {
          for (j = 0; j < number_of_hashes; j++) 
               mhdb[i * number_of_hashes + j] = mh_compute_minhash(&newdb.lists[i], &permutations[j * newdb.dim]);
     }

     double h0[listdb.size][listdb.size];
     uint **collisions = (uint **) calloc(listdb.size, sizeof(uint *));
     for (i = 0; i < listdb.size; i++) 
          collisions[i] = (uint *) calloc(listdb.size, sizeof(uint));

     for (i = 0; i < listdb.size - 1; i++)
          for (j = i + 1; j < listdb.size; j++) {
               h0[j][i] = h0[i][j] = list_histogram_intersection(&listdb.lists[i], &listdb.lists[j]);

               uint k;
               for (k = 0; k < number_of_hashes; k++)
                    if ( mhdb[i * number_of_hashes + k] == mhdb[j * number_of_hashes + k])
                         collisions[i][j]++;
          }
     
     for (i = 0; i < listdb.size - 1; i++)
          for (j = i + 1; j < listdb.size; j++)
               printf("Pair (%d, %d): h0 = %lf, estimation = %lf\n",
                      i, j, h0[i][j], (double) collisions[i][j] /  (double) number_of_hashes);
}

void test_minhash_weighted_items(uint number_of_hashes)
{
     ListDB listdb = listdb_random(50,8,20);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);
               
     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = 1;
     printf("================= Database ==================\n");
     listdb_print(&listdb);

     ListDB ifindex = ifindex_make_from_corpus(&listdb);
     double *weights = (double *) calloc(ifindex.size, sizeof(double));
     for (i = 0; i < ifindex.size; i++) 
          weights[i] = log ((double) listdb.size / (double) ifindex.lists[i].size);

     RandomValue *permutations = (RandomValue *) malloc(number_of_hashes * listdb.dim * sizeof(RandomValue));
     mh_generate_permutations(listdb.dim, number_of_hashes, permutations);
     mh_weight_permutations(listdb.dim, number_of_hashes, permutations, weights);
     
     ullong *mhdb = (ullong *) calloc(number_of_hashes * listdb.size, sizeof(ullong));
     for (i = 0; i < listdb.size; i++) {
          for (j = 0; j < number_of_hashes; j++) 
               mhdb[i * number_of_hashes + j] = mh_compute_minhash(&listdb.lists[i], &permutations[j * listdb.dim]);
     }

     double simw[listdb.size][listdb.size];
     uint **collisions = (uint **) calloc(listdb.size, sizeof(uint *));
     for (i = 0; i < listdb.size; i++) 
          collisions[i] = (uint *) calloc(listdb.size, sizeof(uint));

     for (i = 0; i < listdb.size - 1; i++)
          for (j = i + 1; j < listdb.size; j++) {
               simw[j][i] = simw[i][j] = list_weighted_similarity(&listdb.lists[i], &listdb.lists[j], weights);

               uint k;
               for (k = 0; k < number_of_hashes; k++)
                    if ( mhdb[i * number_of_hashes + k] == mhdb[j * number_of_hashes + k])
                         collisions[i][j]++;
          }
     
     for (i = 0; i < listdb.size - 1; i++)
          for (j = i + 1; j < listdb.size; j++)
               printf("Pair (%d, %d): simw = %lf estimation = %lf\n",
                      i, j, simw[i][j], (double) collisions[i][j] /  (double) number_of_hashes);
}

void test_minhash_frequency_expanded_weighted(uint number_of_hashes)
{
     ListDB listdb = listdb_random(50,8,20);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);
               
     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = rand() % 10 + 1;
     printf("================= Database ==================\n");
     listdb_print(&listdb);

     ListDB ifindex = ifindex_make_from_corpus(&listdb);
     double *oldweights = (double *) calloc(ifindex.size, sizeof(double));
     for (i = 0; i < ifindex.size; i++) 
          oldweights[i] = log ((double) listdb.size / (double) ifindex.lists[i].size);

     uint *maxfreq = mh_get_cumulative_frequency(&listdb, &ifindex);
     ListDB newdb = mh_expand_listdb(&listdb, maxfreq);
     
     double *weights = mh_expand_weights(ifindex.size, maxfreq, oldweights);
     free(maxfreq);
     
     RandomValue *permutations = (RandomValue *) malloc(number_of_hashes * newdb.dim * sizeof(RandomValue));
     mh_generate_permutations(newdb.dim, number_of_hashes, permutations);
     mh_weight_permutations(newdb.dim, number_of_hashes, permutations, weights);
     
     ullong *mhdb = (ullong *) calloc(number_of_hashes * listdb.size, sizeof(ullong));
     for (i = 0; i < listdb.size; i++) {
          for (j = 0; j < number_of_hashes; j++) 
               mhdb[i * number_of_hashes + j] = mh_compute_minhash(&newdb.lists[i], &permutations[j * newdb.dim]);
     }

     double h[listdb.size][listdb.size];
     uint **collisions = (uint **) calloc(listdb.size, sizeof(uint *));
     for (i = 0; i < listdb.size; i++) 
          collisions[i] = (uint *) calloc(listdb.size, sizeof(uint));

     for (i = 0; i < listdb.size - 1; i++)
          for (j = i + 1; j < listdb.size; j++) {
               h[j][i] = h[i][j] = list_weighted_histogram_intersection(&listdb.lists[i], &listdb.lists[j], oldweights);

               uint k;
               for (k = 0; k < number_of_hashes; k++)
                    if ( mhdb[i * number_of_hashes + k] == mhdb[j * number_of_hashes + k]){
                         collisions[j][i]++;
                         collisions[i][j]++;
                    }
          }
     
     for (i = 0; i < listdb.size - 1; i++)
          for (j = i + 1; j < listdb.size; j++)
               printf("Pair (%d, %d): h = %lf, estimation = %lf\n",
                      i, j, h[i][j], (double) collisions[i][j] /  (double) number_of_hashes);
}

void test_minhash_weighted_weighted(uint number_of_hashes)
{
     ListDB listdb = listdb_random(10,8,20);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);               
               
     uint i, j;
     for (i = 0; i < listdb.size; i++) 
          for (j = 0; j < listdb.lists[i].size; j++) 
               listdb.lists[i].data[j].freq = 1;
     printf("================= Database ==================\n");
     listdb_print(&listdb);

     ListDB ifindex = ifindex_make_from_corpus(&listdb);
     ifindex_weight(&listdb, &ifindex, weights_ids);

     RandomValue *permutations = (RandomValue *) malloc(number_of_hashes * listdb.dim * sizeof(RandomValue));
     ullong *mhdb = calloc(number_of_hashes * listdb.size, sizeof(ullong));
     mh_generate_permutations(listdb.dim, number_of_hashes, permutations);

     ListDB hashes = listdb_create(listdb.size, 0);
     for (i = 0; i < listdb.size; i++) {
          for (j = 0; j < number_of_hashes; j++) 
               mhdb[i * number_of_hashes + j] = mh_compute_minhash(&listdb.lists[i], &permutations[j * listdb.dim]);
     }
     
     double *weights = (double *) calloc(ifindex.size, sizeof(double));
     for (i = 0; i < ifindex.size; i++) 
          weights[i] = log ((double) listdb.size / (double) ifindex.lists[i].size);

     
     double simw[listdb.size][listdb.size];
     uint **collisions = (uint **) calloc(listdb.size, sizeof(uint *));
     for (i = 0; i < listdb.size; i++) 
          collisions[i] = (uint *) calloc(listdb.size, sizeof(uint));
     
     for (i = 0; i < listdb.size - 1; i++) 
          for (j = i + 1; j < listdb.size; j++) {
               simw[i][j] = simw[j][i] = list_weighted_similarity(&listdb.lists[i], &listdb.lists[j], weights);

               uint k;
               for (k = 0; k < number_of_hashes; k++)
                    if ( mhdb[i * number_of_hashes + k] == mhdb[j * number_of_hashes + k]) {
                         collisions[j][i]++;
                         collisions[i][j]++;
                    }
          }
     
     for (i = 0; i < listdb.size - 1; i++) 
          for (j = i + 1; j < listdb.size; j++) 
               printf("Pair (%d, %d): h0 = %lf, estimator = %lf\n",
                      i, j, simw[i][j], (double) collisions[i][j] /  (double) number_of_hashes);
}

void test_minhash_frequency_weighted(uint number_of_hashes)
{
     ListDB listdb = listdb_random(10,8,20);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);               
               
     uint i, j;
     for (i = 0; i < listdb.size; i++) 
          for (j = 0; j < listdb.lists[i].size; j++) 
               listdb.lists[i].data[j].freq = rand() % 10 + 1;
     printf("================= Database ==================\n");
     listdb_print(&listdb);

     RandomValue *permutations = (RandomValue *) malloc(number_of_hashes * listdb.dim * sizeof(RandomValue));
     ullong *mhdb = calloc(number_of_hashes * listdb.size, sizeof(ullong));
     mh_generate_permutations(listdb.dim, number_of_hashes, permutations);

     ListDB hashes = listdb_create(listdb.size, 0);
     for (i = 0; i < listdb.size; i++) {
          for (j = 0; j < number_of_hashes; j++) 
               mhdb[i * number_of_hashes + j] = mh_compute_minhash(&listdb.lists[i], &permutations[j * listdb.dim]);
     }
     
     ListDB ifindex = ifindex_make_from_corpus(&listdb);
     double *weights = (double *) calloc(ifindex.size, sizeof(double));
     for (i = 0; i < ifindex.size; i++) 
          weights[i] = log ((double) listdb.size / (double) ifindex.lists[i].size);

     
     double h0[listdb.size][listdb.size];
     uint **collisions = (uint **) calloc(listdb.size, sizeof(uint *));
     for (i = 0; i < listdb.size; i++) 
          collisions[i] = (uint *) calloc(listdb.size, sizeof(uint));
     
     for (i = 0; i < listdb.size - 1; i++) 
          for (j = i + 1; j < listdb.size; j++) {
               h0[i][j] = list_histogram_intersection(&listdb.lists[i], &listdb.lists[j]);

               uint k;
               for (k = 0; k < number_of_hashes; k++)
                    if ( mhdb[i * number_of_hashes + k] == mhdb[j * number_of_hashes + k]) {
                         collisions[j][i]++;
                         collisions[i][j]++;
                    }
          }
     
     for (i = 0; i < listdb.size - 1; i++) 
          for (j = i + 1; j < listdb.size; j++) 
               printf("Pair (%d, %d): h0 = %lf, estimator = %lf\n",
                      i, j, h0[i][j], (double) collisions[i][j] /  (double) number_of_hashes);
}

void test_minhash_frequency_weighted_weighted(uint number_of_hashes)
{
     ListDB listdb = listdb_random(10,8,20);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);               
               
     uint i, j;
     for (i = 0; i < listdb.size; i++) 
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = rand() % 10 + 1;
     printf("================= Database ==================\n");
     listdb_print(&listdb);

     ListDB ifindex = ifindex_make_from_corpus(&listdb);
     ifindex_weight(&listdb, &ifindex, weights_tfids);

     RandomValue *permutations = (RandomValue *) malloc(number_of_hashes * listdb.dim * sizeof(RandomValue));
     ullong *mhdb = calloc(number_of_hashes * listdb.size, sizeof(ullong));
     mh_generate_permutations(listdb.dim, number_of_hashes, permutations);

     ListDB hashes = listdb_create(listdb.size, 0);
     for (i = 0; i < listdb.size; i++) {
          for (j = 0; j < number_of_hashes; j++) 
               mhdb[i * number_of_hashes + j] = mh_compute_minhash(&listdb.lists[i], &permutations[j * listdb.dim]);
     }
     
     double *weights = (double *) calloc(ifindex.size, sizeof(double));
     for (i = 0; i < ifindex.size; i++) 
          weights[i] = log ((double) listdb.size / (double) ifindex.lists[i].size);

     
     double h[listdb.size][listdb.size];
     uint **collisions = (uint **) calloc(listdb.size, sizeof(uint *));
     for (i = 0; i < listdb.size; i++) 
          collisions[i] = (uint *) calloc(listdb.size, sizeof(uint));
     
     for (i = 0; i < listdb.size - 1; i++) 
          for (j = i + 1; j < listdb.size; j++) {
               h[j][i] = h[i][j] = list_weighted_histogram_intersection(&listdb.lists[i], &listdb.lists[j], weights);

               uint k;
               for (k = 0; k < number_of_hashes; k++)
                    if ( mhdb[i * number_of_hashes + k] == mhdb[j * number_of_hashes + k]) {
                         collisions[j][i]++;
                         collisions[i][j]++;
                    }
          }
     
     for (i = 0; i < listdb.size - 1; i++) 
          for (j = i + 1; j < listdb.size; j++) 
               printf("Pair (%d, %d): h0 = %lf, estimator = %lf\n",
                      i, j, h[i][j], (double) collisions[i][j] /  (double) number_of_hashes);
}

int main(int argc, char **argv)
{
     srand(123456);
     
     /* test_init_create_print(); */
     /* test_hash(); */
     /* test_store(); */
     /* test_minhash_binary(1000000); */
     /* test_minhash_frequency_expanded(100000); */
     /* test_minhash_weighted_items(100000); */
     /* test_minhash_frequency_expanded_weighted(100000); */
     /* test_minhash_frequency_weighted(1000000); */
     /* test_minhash_weighted_weighted(1000000); */
     test_minhash_frequency_weighted_weighted(1000000);
 
     return 0;
}
