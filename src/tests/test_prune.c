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

void test_prune(char *iffile, char *csfile, char *output, 
		uint stop, uint dochits, double ovr_perc, double cooc_perc)
{
     // load inverted file and mined sets
     ListDB ifindex = listdb_load_from_file(iffile);
     ListDB mined = listdb_load_from_file(csfile);

     sampledmh_prune(&ifindex, &mined, stop, dochits, ovr_perc, cooc_perc);
     listdb_print(&mined);
     listdb_save_to_file(output, &mined);
}

int main(int argc, char **argv)
{
     srand((long int) time(NULL));
     test_prune(argv[1], argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atof(argv[6]), atof(argv[7]));
	 
     return 0;
}
