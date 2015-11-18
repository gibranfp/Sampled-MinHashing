#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "weights.h"
#include "ifindex.h"
#include "minhash.h"
#include "sampledmh.h"

#define red "\033[0;31m"
#define cyan "\033[0;36m"
#define green "\033[0;32m"
#define blue "\033[0;34m"
#define brown "\033[0;33m"
#define magenta "\033[0;35m"
#define none "\033[0m"

#define MAX_LIST_SIZE 10
#define ELEMENT_MAX_VALUE 15

void test_mine(uint tuple_size, uint number_of_tuples, uint table_size)
{
     ListDB listdb = listdb_random(20,8,15);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);
               
     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = 1;
     printf("========= List Database ========\n");
     listdb_print(&listdb);
     printf("\n");

     ListDB coitems = sampledmh_mine(&listdb, tuple_size, number_of_tuples, table_size);
     listdb_sort_by_size_back(&coitems);
     listdb_delete_smallest(&coitems, 3);

     uint *repeat = (uint *) calloc(coitems.size, sizeof(uint));
     for (i = 0; i < coitems.size - 1; i++) {
          repeat[i]++;
          for (j = i + 1; j < coitems.size; j++) {
               double ovr = list_overlap(&coitems.lists[i], &coitems.lists[j]);
               if( ovr == 1.0 ) {
                    if ( coitems.lists[i].size <= coitems.lists[j].size )
                         repeat[i]++;

                    if ( coitems.lists[i].size >= coitems.lists[j].size )
                         repeat[j]++;
               }
          }
     }
     repeat[i]++;
     
     double *jcc = (double *) calloc(coitems.size, sizeof(double *));
     for (i = 0; i < coitems.size; i++) {
          List inter = list_duplicate(&listdb.lists[coitems.lists[i].data[0].item]);
          List uni = list_duplicate(&listdb.lists[coitems.lists[i].data[0].item]);
          for (j = 1; j < coitems.lists[i].size; j++){
               List tmp = list_intersection(&inter, &listdb.lists[coitems.lists[i].data[j].item]);

               list_destroy(&inter);
               inter = list_duplicate(&tmp);
               list_destroy(&tmp);

               tmp = list_union(&uni, &listdb.lists[coitems.lists[i].data[j].item]);
               list_destroy(&uni);
               uni = list_duplicate(&tmp);
               list_destroy(&tmp);
          }

          jcc[i] = pow((double) inter.size / (double) uni.size, (double) tuple_size);

          list_destroy(&inter);
          list_destroy(&uni);
     }

     printf("========= Mined lists ========\n");
     for (i = 0; i < coitems.size; i++) {
          printf("[   %u   ] ( %lf = %u %lf ) %u -- ", i, jcc[i], repeat[i], (double) repeat[i] /  (double) number_of_tuples, coitems.lists[i].size);
          for (j = 0; j < coitems.lists[i].size; j++)
               printf("%u ", coitems.lists[i].data[j].item);
          printf("\n");
     }
}

void test_mine_weighted(uint tuple_size, uint number_of_tuples, uint table_size)
{
     ListDB listdb = listdb_random(20,8,15);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);
               
     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = 1;
     printf("========= List Database ========\n");
     listdb_print(&listdb);
     printf("\n");

     ListDB ifindex = ifindex_make_from_corpus(&listdb);
     double *weights = (double *) calloc(ifindex.size, sizeof(double));
     for (i = 0; i < ifindex.size; i++)
          weights[i] = log ((double) listdb.size / (double) ifindex.lists[i].size);

     ListDB coitems = sampledmh_mine_weighted(&listdb, tuple_size, number_of_tuples,
                                              table_size, weights);
     listdb_sort_by_size_back(&coitems);
     listdb_delete_smallest(&coitems, 3);

     uint *repeat = (uint *) calloc(coitems.size, sizeof(uint));
     for (i = 0; i < coitems.size - 1; i++) {
          repeat[i]++;
          for (j = i + 1; j < coitems.size; j++) {
               double ovr = list_overlap(&coitems.lists[i], &coitems.lists[j]);
               if( ovr == 1.0 ) {
                    if ( coitems.lists[i].size <= coitems.lists[j].size )
                         repeat[i]++;

                    if ( coitems.lists[i].size >= coitems.lists[j].size )
                         repeat[j]++;
               }
          }
     }
     repeat[i]++;
     
     double *wcc = (double *) calloc(coitems.size, sizeof(double *));
     for (i = 0; i < coitems.size; i++) {
          List inter = list_duplicate(&listdb.lists[coitems.lists[i].data[0].item]);
          List uni = list_duplicate(&listdb.lists[coitems.lists[i].data[0].item]);
          for (j = 1; j < coitems.lists[i].size; j++){
               List tmp = list_intersection(&inter, &listdb.lists[coitems.lists[i].data[j].item]);

               list_destroy(&inter);
               inter = list_duplicate(&tmp);
               list_destroy(&tmp);

               tmp = list_union(&uni, &listdb.lists[coitems.lists[i].data[j].item]);
               list_destroy(&uni);
               uni = list_duplicate(&tmp);
               list_destroy(&tmp);
          }

          double winter = 0.0;
          for (j = 0; j < inter.size; j++)
               winter += weights[inter.data[j].item];

          double wuni = 0.0;
          for (j = 0; j < uni.size; j++)
               wuni += weights[uni.data[j].item];

          wcc[i] = pow(winter / wuni, (double) tuple_size);

          list_destroy(&inter);
          list_destroy(&uni);
     }

     printf("========= Mined lists ========\n");
     for (i = 0; i < coitems.size; i++) {
          printf("[   %u   ] ( %lf = %u %lf ) %u -- ", i, wcc[i], repeat[i], (double) repeat[i] /  (double) number_of_tuples, coitems.lists[i].size);
          for (j = 0; j < coitems.lists[i].size; j++)
               printf("%u ", coitems.lists[i].data[j].item);
          printf("\n");
     }
}

void test_mine_frequency_expanded(uint tuple_size, uint number_of_tuples, uint table_size)
{
     ListDB listdb = listdb_random(20,8,15);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);
               
     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = rand() % 10 + 1;
     printf("========= List Database ========\n");
     listdb_print(&listdb);
     printf("\n");

     ListDB ifindex = ifindex_make_from_corpus(&listdb);
     uint *maxfreq = mh_get_cumulative_frequency(&listdb, &ifindex);
     ListDB expldb = mh_expand_listdb(&listdb, maxfreq);
     
     ListDB coitems = sampledmh_mine(&expldb, tuple_size, number_of_tuples, table_size);
     listdb_sort_by_size_back(&coitems);
     listdb_delete_smallest(&coitems, 3);

     uint *repeat = (uint *) calloc(coitems.size, sizeof(uint));
     for (i = 0; i < coitems.size - 1; i++) {
          repeat[i]++;
          for (j = i + 1; j < coitems.size; j++) {
               double ovr = list_overlap(&coitems.lists[i], &coitems.lists[j]);
               if( ovr == 1.0 ) {
                    if ( coitems.lists[i].size <= coitems.lists[j].size )
                         repeat[i]++;

                    if ( coitems.lists[i].size >= coitems.lists[j].size )
                         repeat[j]++;
               }
          }
     }
     repeat[i]++;
     
     double *fcc = (double *) calloc(coitems.size, sizeof(double *));
     for (i = 0; i < coitems.size; i++) {
          List inter = list_duplicate(&listdb.lists[coitems.lists[i].data[0].item]);
          List uni = list_duplicate(&listdb.lists[coitems.lists[i].data[0].item]);
          for (j = 1; j < coitems.lists[i].size; j++){
               List tmp = list_intersection(&inter, &listdb.lists[coitems.lists[i].data[j].item]);

               list_destroy(&inter);
               inter = list_duplicate(&tmp);
               list_destroy(&tmp);

               tmp = list_union(&uni, &listdb.lists[coitems.lists[i].data[j].item]);
               list_destroy(&uni);
               uni = list_duplicate(&tmp);
               list_destroy(&tmp);
          }

          uint sum_inter = list_sum_freq(&inter);
          uint sum_uni = list_sum_freq(&uni);
          
          fcc[i] = pow((double) sum_inter / (double) sum_uni, (double) tuple_size);

          list_destroy(&inter);
          list_destroy(&uni);
     }

     printf("========= Mined lists ========\n");
     for (i = 0; i < coitems.size; i++) {
          printf("[   %u   ] ( %lf = %u %lf ) %u -- ", i, fcc[i], repeat[i], (double) repeat[i] /  (double) number_of_tuples, coitems.lists[i].size);
          for (j = 0; j < coitems.lists[i].size; j++)
               printf("%u ", coitems.lists[i].data[j].item);
          printf("\n");
     }
}

void test_mine_frequency_expanded_weighted(uint tuple_size, uint number_of_tuples, uint table_size)
{
     ListDB listdb = listdb_random(20,8,15);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);
               
     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = rand() % 10 + 1;
     printf("========= List Database ========\n");
     listdb_print(&listdb);

     ListDB ifindex = ifindex_make_from_corpus(&listdb);
     uint *maxfreq = mh_get_cumulative_frequency(&listdb, &ifindex);
     ListDB expldb = mh_expand_listdb(&listdb, maxfreq);
     
     double *weights = (double *) calloc(ifindex.size, sizeof(double));
     for (i = 0; i < ifindex.size; i++)
          weights[i] = log ((double) listdb.size / (double) ifindex.lists[i].size);

     /* printf("\t   "); */
     /* for (i = 0; i < ifindex.size; i++) { */
     /*      printf("%2.4lf  ", weights[i]); */
     /* } */
     /* printf("\n"); */

     /* for (i = 0; i < listdb.size; i++) { */
     /*      printf("%2u(%2u) | ", i, listdb.lists[i].size); */
     /*      uint k = 0; */
     /*      for (j = 0; j < listdb.lists[i].size; j++) { */
     /*           while (listdb.lists[i].data[j].item > k){ */
     /*                printf("%6u  ", 0); */
     /*                k++; */
     /*           } */
     /*           printf("%6u  ", listdb.lists[i].data[j].freq); */
     /*           k++; */
     /*      } */
     /*      while (k < listdb.dim){ */
     /*           printf("%6u  ", 0); */
     /*           k++; */
     /*      } */
     /*      printf("\n"); */
     /* } */

     ListDB coitems = sampledmh_mine_weighted(&expldb, tuple_size, number_of_tuples,
                                              table_size, weights);
     listdb_sort_by_size_back(&coitems);
     listdb_delete_smallest(&coitems, 3);

     uint *repeat = (uint *) calloc(coitems.size, sizeof(uint));
     for (i = 0; i < coitems.size - 1; i++) {
          repeat[i]++;
          for (j = i + 1; j < coitems.size; j++) {
               double ovr = list_overlap(&coitems.lists[i], &coitems.lists[j]);
               if( ovr >= 1.0 ) {
                    if ( coitems.lists[i].size <= coitems.lists[j].size )
                         repeat[i]++;

                    if ( coitems.lists[i].size >= coitems.lists[j].size )
                         repeat[j]++;
               }
          }
     }
     repeat[coitems.size - 1]++;

     double *wfcc = (double *) calloc(coitems.size, sizeof(double *));
     for (i = 0; i < coitems.size; i++) {
          List inter = list_duplicate(&listdb.lists[coitems.lists[i].data[0].item]);
          List uni = list_duplicate(&listdb.lists[coitems.lists[i].data[0].item]);
          for (j = 1; j < coitems.lists[i].size; j++){
               List tmp = list_intersection(&inter, &listdb.lists[coitems.lists[i].data[j].item]);

               list_destroy(&inter);
               inter = list_duplicate(&tmp);
               list_destroy(&tmp);

               tmp = list_union(&uni, &listdb.lists[coitems.lists[i].data[j].item]);
               list_destroy(&uni);
               uni = list_duplicate(&tmp);
               list_destroy(&tmp);
          }

          /* printf("Coocc %u:\n",i); */
          /* for (j = 0; j < coitems.lists[i].size; j++) { */
          /*      printf("%2u(%2u) | ", coitems.lists[i].data[j].item, listdb.lists[coitems.lists[i].data[j].item].size); */
          /*      uint k = 0; */
          /*      uint h; */
          /*      for (h = 0; h < listdb.lists[coitems.lists[i].data[j].item].size; h++) { */
          /*           while (listdb.lists[coitems.lists[i].data[j].item].data[h].item > k){ */
          /*                printf("%6u  ", 0); */
          /*                k++; */
          /*           } */
          /*           printf("%6u  ", listdb.lists[coitems.lists[i].data[j].item].data[h].freq); */
          /*           k++; */
          /*      } */
          /*      while (k < listdb.dim){ */
          /*           printf("%6u  ", 0); */
          /*           k++; */
          /*      } */
          /*      printf("\n"); */
          /* } */

          /* printf("Inter %u:\t",i); */
          /* for (i = 0; i < inter.size; i++) { */
          /*      printf("%u * %lf + ", inter.data[i].freq, weights[inter.data[i].item]); */
          /* } */
          /* printf("\n"); */
          /* printf("Uni %u:\t",i); */
          /* for (i = 0; i < uni.size; i++) { */
          /*      printf("%u * %lf + ", uni.data[i].freq, weights[uni.data[i].item]); */
          /* } */
          /* printf("\n"); */
          
          double winter = 0.0;
          for (j = 0; j < inter.size; j++)
               winter += weights[inter.data[j].item] * (double) inter.data[j].freq;

          double wuni = 0.0;
          for (j = 0; j < uni.size; j++)
               wuni += weights[uni.data[j].item] * (double) uni.data[j].freq;

          wfcc[i] = pow(winter / wuni, (double) tuple_size);

          /* printf("Weigtht %lf\n ", wfcc[i]); */
          list_destroy(&inter);
          list_destroy(&uni);
     }

     printf("========= Mined lists ========\n");
     for (i = 0; i < coitems.size; i++) {
          printf("[   %u   ] ( %lf = %u %lf ) %u -- ", i, wfcc[i], repeat[i], (double) repeat[i] /  (double) number_of_tuples, coitems.lists[i].size);
          for (j = 0; j < coitems.lists[i].size; j++)
               printf("%u ", coitems.lists[i].data[j].item);
          printf("\n");
     }
}

void test_mine_frequency_weighted(uint tuple_size, uint number_of_tuples, uint table_size)
{
     ListDB listdb = listdb_random(20,8,15);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);
               
     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = rand() % 10 + 1;
     printf("========= List Database ========\n");
     listdb_print(&listdb);
     printf("\n");
     
     ListDB coitems = sampledmh_mine(&listdb, tuple_size, number_of_tuples, table_size);
     listdb_sort_by_size_back(&coitems);
     listdb_delete_smallest(&coitems, 3);

     uint *repeat = (uint *) calloc(coitems.size, sizeof(uint));
     for (i = 0; i < coitems.size - 1; i++) {
          repeat[i]++;
          for (j = i + 1; j < coitems.size; j++) {
               double ovr = list_overlap(&coitems.lists[i], &coitems.lists[j]);
               if( ovr == 1.0 ) {
                    if ( coitems.lists[i].size <= coitems.lists[j].size )
                         repeat[i]++;

                    if ( coitems.lists[i].size >= coitems.lists[j].size )
                         repeat[j]++;
               }
          }
     }
     repeat[i]++;
     
     double *fcc = (double *) calloc(coitems.size, sizeof(double *));
     for (i = 0; i < coitems.size; i++) {
          List inter = list_duplicate(&listdb.lists[coitems.lists[i].data[0].item]);
          List uni = list_duplicate(&listdb.lists[coitems.lists[i].data[0].item]);
          for (j = 1; j < coitems.lists[i].size; j++){
               List tmp = list_intersection(&inter, &listdb.lists[coitems.lists[i].data[j].item]);

               list_destroy(&inter);
               inter = list_duplicate(&tmp);
               list_destroy(&tmp);

               tmp = list_union(&uni, &listdb.lists[coitems.lists[i].data[j].item]);
               list_destroy(&uni);
               uni = list_duplicate(&tmp);
               list_destroy(&tmp);
          }

          uint sum_inter = list_sum_freq(&inter);
          uint sum_uni = list_sum_freq(&uni);
          
          fcc[i] = pow((double) sum_inter / (double) sum_uni, (double) tuple_size);

          list_destroy(&inter);
          list_destroy(&uni);
     }

     printf("========= Mined lists ========\n");
     for (i = 0; i < coitems.size; i++) {
          printf("[   %u   ] ( %lf = %u %lf ) %u -- ", i, fcc[i], repeat[i], (double) repeat[i] /  (double) number_of_tuples, coitems.lists[i].size);
          for (j = 0; j < coitems.lists[i].size; j++)
               printf("%u ", coitems.lists[i].data[j].item);
          printf("\n");
     }
}

void test_mine_weighted_weighted(uint tuple_size, uint number_of_tuples, uint table_size)
{
     ListDB listdb = listdb_random(20,8,15);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);
               
     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = 1;
     printf("========= List Database ========\n");
     listdb_print(&listdb);
     printf("\n");

     ListDB ifindex = ifindex_make_from_corpus(&listdb);
     ifindex_weight(&listdb, &ifindex, weights_ids);
     
     double *weights = (double *) calloc(ifindex.size, sizeof(double));
     for (i = 0; i < ifindex.size; i++)
          weights[i] = log ((double) listdb.size / (double) ifindex.lists[i].size);

     ListDB coitems = sampledmh_mine(&listdb, tuple_size, number_of_tuples, table_size);
     listdb_sort_by_size_back(&coitems);
     listdb_delete_smallest(&coitems, 3);

     uint *repeat = (uint *) calloc(coitems.size, sizeof(uint));
     for (i = 0; i < coitems.size - 1; i++) {
          repeat[i]++;
          for (j = i + 1; j < coitems.size; j++) {
               double ovr = list_overlap(&coitems.lists[i], &coitems.lists[j]);
               if( ovr == 1.0 ) {
                    if ( coitems.lists[i].size <= coitems.lists[j].size )
                         repeat[i]++;

                    if ( coitems.lists[i].size >= coitems.lists[j].size )
                         repeat[j]++;
               }
          }
     }
     repeat[i]++;
     
     double *wcc = (double *) calloc(coitems.size, sizeof(double *));
     for (i = 0; i < coitems.size; i++) {
          List inter = list_duplicate(&listdb.lists[coitems.lists[i].data[0].item]);
          List uni = list_duplicate(&listdb.lists[coitems.lists[i].data[0].item]);
          for (j = 1; j < coitems.lists[i].size; j++){
               List tmp = list_intersection(&inter, &listdb.lists[coitems.lists[i].data[j].item]);

               list_destroy(&inter);
               inter = list_duplicate(&tmp);
               list_destroy(&tmp);

               tmp = list_union(&uni, &listdb.lists[coitems.lists[i].data[j].item]);
               list_destroy(&uni);
               uni = list_duplicate(&tmp);
               list_destroy(&tmp);
          }

          double winter = 0.0;
          for (j = 0; j < inter.size; j++)
               winter += weights[inter.data[j].item];

          double wuni = 0.0;
          for (j = 0; j < uni.size; j++)
               wuni += weights[uni.data[j].item];

          wcc[i] = pow(winter / wuni, (double) tuple_size);

          list_destroy(&inter);
          list_destroy(&uni);
     }

     printf("========= Mined lists ========\n");
     for (i = 0; i < coitems.size; i++) {
          printf("[   %u   ] ( %lf = %u %lf ) %u -- ", i, wcc[i], repeat[i], (double) repeat[i] /  (double) number_of_tuples, coitems.lists[i].size);
          for (j = 0; j < coitems.lists[i].size; j++)
               printf("%u ", coitems.lists[i].data[j].item);
          printf("\n");
     }
}

void test_mine_frequency_weighted_weighted(uint tuple_size, uint number_of_tuples, uint table_size)
{
     ListDB listdb = listdb_random(20,8,15);
     listdb_delete_smallest(&listdb, 3);
     listdb_apply_to_all(&listdb, list_sort_by_item);
     listdb_apply_to_all(&listdb, list_unique);
               
     uint i, j;
     for (i = 0; i < listdb.size; i++)
          for (j = 0; j < listdb.lists[i].size; j++)
               listdb.lists[i].data[j].freq = rand() % 10 + 1;
     printf("========= List Database ========\n");
     listdb_print(&listdb);
     printf("\n");

     ListDB ifindex = ifindex_make_from_corpus(&listdb);
     ifindex_weight(&listdb, &ifindex, weights_tfids);
     
     double *weights = (double *) calloc(ifindex.size, sizeof(double));
     for (i = 0; i < ifindex.size; i++)
          weights[i] = log ((double) listdb.size / (double) ifindex.lists[i].size);

     ListDB coitems = sampledmh_mine(&listdb, tuple_size, number_of_tuples, table_size);
     listdb_sort_by_size_back(&coitems);
     listdb_delete_smallest(&coitems, 3);

     uint *repeat = (uint *) calloc(coitems.size, sizeof(uint));
     for (i = 0; i < coitems.size - 1; i++) {
          repeat[i]++;
          for (j = i + 1; j < coitems.size; j++) {
               double ovr = list_overlap(&coitems.lists[i], &coitems.lists[j]);
               if( ovr == 1.0 ) {
                    if ( coitems.lists[i].size <= coitems.lists[j].size )
                         repeat[i]++;

                    if ( coitems.lists[i].size >= coitems.lists[j].size )
                         repeat[j]++;
               }
          }
     }
     repeat[i]++;
     
     double *wfcc = (double *) calloc(coitems.size, sizeof(double *));
     for (i = 0; i < coitems.size; i++) {
          List inter = list_duplicate(&listdb.lists[coitems.lists[i].data[0].item]);
          List uni = list_duplicate(&listdb.lists[coitems.lists[i].data[0].item]);
          for (j = 1; j < coitems.lists[i].size; j++){
               List tmp = list_intersection(&inter, &listdb.lists[coitems.lists[i].data[j].item]);

               list_destroy(&inter);
               inter = list_duplicate(&tmp);
               list_destroy(&tmp);

               tmp = list_union(&uni, &listdb.lists[coitems.lists[i].data[j].item]);
               list_destroy(&uni);
               uni = list_duplicate(&tmp);
               list_destroy(&tmp);
          }

          double winter = 0.0;
          for (j = 0; j < inter.size; j++)
               winter += weights[inter.data[j].item] * ((double) inter.data[j].freq);

          double wuni = 0.0;
          for (j = 0; j < uni.size; j++)
               wuni += weights[uni.data[j].item] * ((double) uni.data[j].freq);

          wfcc[i] = pow(winter / wuni, (double) tuple_size);

          list_destroy(&inter);
          list_destroy(&uni);
     }

     printf("========= Mined lists ========\n");
     for (i = 0; i < coitems.size; i++) {
          printf("[   %u   ] ( %lf = %u %lf ) %u -- ", i, wfcc[i], repeat[i], (double) repeat[i] /  (double) number_of_tuples, coitems.lists[i].size);
          for (j = 0; j < coitems.lists[i].size; j++)
               printf("%u ", coitems.lists[i].data[j].item);
          printf("\n");
     }
}

int main(int argc, char **argv)
{
  srand((long int) time(NULL));
     /* test_mine(4, 100000, 1024); */
     /* test_mine_weighted(3, 100000, 1024); */
     /* test_mine_frequency_expanded(3, 10000, 1024); */
     test_mine_frequency_expanded_weighted(1, 1000, 1024);
     /* test_mine_frequency_weighted(3, 100000, 1024); */
     /* test_mine_weighted_weighted(1, 10000, 1024); */
     /* test_mine_frequency_weighted_weighted(1, 1000, 1024); */
     
     return 0;
}
