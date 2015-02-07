#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "ifindex.h"
#include "minhash.h"
#include "sampledmh.h"

#define red   "\033[0;31m"        /* 0 -> normal ;  31 -> red */
#define cyan  "\033[0;36m"        /* 1 -> bold ;  36 -> cyan */
#define green "\033[0;32m"        /* 4 -> underline ;  32 -> green */
#define blue  "\033[0;34m"        /* 9 -> strike ;  34 -> blue */
 
#define black  "\033[0;30m"
#define brown  "\033[0;33m"
#define magenta  "\033[0;35m"
#define gray  "\033[0;37m"
 
#define none   "\033[0m"        /* to flush the previous property */

#define MAX_LIST_SIZE 10
#define ELEMENT_MAX_VALUE 15

void test_mine(char *filename, char *output, uint tuple_size, uint number_of_tuples, uint table_size)
{
	ListDB listdb = listdb_load_from_file(filename);
	ListDB coitems = sampledmh_mine(&listdb, tuple_size, number_of_tuples, table_size);

	listdb_sort_by_size_back(&coitems);
	listdb_save_to_file(output,&coitems);
}

int main(int argc, char **argv)
{
     srand((long int) time(NULL));
	 test_mine(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
	 
     return 0;
}
