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
	listdb_save_to_file(output,&coitems);
}

int main(int argc, char **argv)
{
     srand((long int) time(NULL));
	 test_mine(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
	 
     return 0;
}
