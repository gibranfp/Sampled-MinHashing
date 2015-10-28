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

void test_prune(uint stop, uint dochits, double ovr_perc, double cooc_perc)
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

     printf("========= Inverted file index ========\n");
     ListDB ifindex = ifindex_make_from_corpus(&listdb);
     listdb_print(&ifindex);
     
     sampledmh_prune(&ifindex, &listdb, stop, dochits, ovr_perc, cooc_perc);

     printf("========= Pruned database ========\n");
     listdb_print(&listdb);
}

int main(int argc, char **argv)
{
     /* srand((long int) time(NULL)); */
     srand(1234566);
     test_prune(atoi(argv[1]), atoi(argv[2]), atof(argv[3]), atof(argv[4]));
	 
     return 0;
}
