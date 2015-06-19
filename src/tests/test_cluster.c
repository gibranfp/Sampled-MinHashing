#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "ifindex.h"
#include "minhash.h"
#include "sampledmh.h"
#include "array_lists.h"
#include "mhlink.h"

#define red "\033[0;31m"
#define cyan "\033[0;36m"
#define green "\033[0;32m"
#define blue "\033[0;34m"
#define brown "\033[0;33m"
#define magenta "\033[0;35m"
#define none "\033[0m"

void test_cluster(char *input, char *output, char *modfile, uint table_size, uint number_of_tuples, uint tuple_size, double thres)
{
	// load inverted file and mined sets
	ListDB mined = listdb_load_from_file(input);
	ListDB clusters = mhlink_cluster(&mined, table_size, number_of_tuples, tuple_size, list_overlap, thres);
	listdb_save_to_file(output, &clusters);

	ListDB models = mhlink_make_model(&mined, &clusters);
	listdb_save_to_file(modfile, &models);
}

int main(int argc, char **argv)
{
     srand((long int) time(NULL));
     test_cluster(argv[1], argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atof(argv[7]));
     return 0;
}
