#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
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

void test_mine(char *filename, char *output, uint tuple_size, uint number_of_tuples, uint table_size)
{
     ListDB listdb = listdb_load_from_file(filename);
     ListDB coitems = sampledmh_mine(&listdb, tuple_size, number_of_tuples, table_size);

     listdb_sort_by_size_back(&coitems);
     listdb_save_to_file(output, &coitems);
}

void test_check(uint tuple_size, uint number_of_tuples, uint table_size)
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

     ListDB coitems = sampledmh_mine(&listdb, tuple_size, number_of_tuples, table_size);
     listdb_delete_smallest(&coitems, 2);
     listdb_apply_to_all(&coitems, list_sort_by_item);

     double dist[listdb.size][listdb.size];
     uint collisions[listdb.size][listdb.size];

     for (i = 0; i < listdb.size - 1; i++) 
          for (j = i + 1; j < listdb.size; j++) 
               dist[i][j] = list_jaccard(&listdb.lists[i], &listdb.lists[j]);

     uint k;
     for (i = 0; i < coitems.size; i++) 
          for (j = 0; j < coitems.lists[i].size - 1; j++)
               for (k = j + 1; k < coitems.lists[i].size; k++){ 
                    collisions[coitems.lists[i].data[j].item][coitems.lists[i].data[k].item]++;
                    collisions[coitems.lists[i].data[k].item][coitems.lists[i].data[j].item]++;
               }

     for (i = 0; i < listdb.size - 1; i++) 
          for (j = i + 1; j < listdb.size; j++) 
               printf("Pair (%d, %d): sim = %lf,  P_{collision} = %lf,"
                      " collisions = %u, avg collisions = %lf\n",
                      i, j, dist[i][j],
                      pow(dist[i][j], tuple_size), collisions[i][j],
                      (double) collisions[i][j] / (double) number_of_tuples);
}

int main(int argc, char **argv)
{
     srand((long int) time(NULL));
     //test_mine(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
     test_check(3, 100000, 1024);
	 
     return 0;
}
