#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "ifindex.h"

#define red "\033[0;31m"
#define cyan "\033[0;36m"
#define green "\033[0;32m"
#define blue "\033[0;34m"
#define brown "\033[0;33m"
#define magenta "\033[0;35m"
#define none "\033[0m"

#define MAX_LIST_SIZE 10
#define ELEMENT_MAX_VALUE 15

void test_query(void)
{
     ListDB ifindex = listdb_random(10, MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
     ListDB queries = listdb_random(3, 5, 10);
     
     listdb_apply_to_all(&ifindex, list_sort_by_item);
     listdb_apply_to_all(&ifindex, list_unique);
     listdb_apply_to_all(&queries, list_sort_by_item);
     listdb_apply_to_all(&queries, list_unique);
     
     printf("%s=========================\nInverted File\n=========================\n", red);
     listdb_print(&ifindex);
     
     printf("%s=========================\nQueries\n=========================\n", brown);
     listdb_print(&queries);
      
     ListDB results = ifindex_query_multi(&ifindex, &queries);
     printf("\n%s=========================\nResult\n=========================\n", cyan);
     listdb_print(&results);

     printf("%s", none);
}

int main()
{
     srand((long int) time(NULL));
     
     test_query();
     
     return 0;
}
