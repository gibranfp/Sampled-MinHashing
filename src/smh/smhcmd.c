/**
 * @file mhclus.c 
 * @author Gibran Fuentes-Pineda <gibranfp@unam.mx>
 * @date 2017
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
 * @brief This program performs MinHash based clustering on dyadic data.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <getopt.h>
#include <inttypes.h>
#include "mt64.h"
#include "weights.h"
#include "ifindex.h"
#include "sampledmh.h"
#include "mhlink.h"

enum WeightScheme {TF, LOGTF, BINTF, IDF, IDS, TFIDF, TFIDS};

/**
 * @brief Prints help in screen.
 */
void usage(void)
{
     printf("usage: smhcmd ifindex [OPTIONS]... [INPUT_FILE] [OUTPUT_FILE]\n"
            "       smhcmd weights [OPTIONS]... [CORPUS_FILE] [INVERTED_FILE] [WEIGHTS_FILE]\n"
            "       smhcmd discover [OPTIONS]... [INPUT_FILE] [OUTPUT_FILE]\n"
            "Creates inverted file structure from corpus, computes weights and discovers patterns\n\n"
            "General options:\n"
            "   --help\t\tPrints this help\n"
            "weights options:\n"
            "   -w, --weight[=idf]\tWeighting scheme to use\n"
            "discover options:\n"
            "   -r, --tuple_size[=4]\tNumber of hashes per tuple in mining phase\n"
            "   -l, --number_of_tuples[=500]\tNumber of tuples in mining phase\n"
            "   -t, --table_size[=20(2^20 = 1048576)]\tNumber of buckets in hash"
            "                                          table (power of 2) in mining phase\n"
            "   -s, --min_set_size[=3]\t Minimum size of set to mine\n"
            "   -x, --cluster_tuple_size[=3]\tNumber of hashes per tuple in clustering phase\n"
            "   -y, --cluster_number_of_tuples[=255]\tNumber of tuples in clustering phase\n"
            "   -z, --cluster_table_size[=1048576]\tNumber of buckets in hash"
            "                                       table (power of 2) in clustering phase\n"
            "   -o, --overlap[=0.7]\tOverlap threshold for clustering phase\n"
            "   -c, --min_cluster_size[=3]\t Minimum size of cluster to consider as meaningful\n"
            "   -e, --expand[=NULL]\t Corpus file used to consider frequencies\n"
            "   -w, --weights[=NULL]\t Weights file used to consider item weights \n");
}

/**
 * @brief Creates inverted file from corpus file.
 *
 * @param opnum Number of command line options.
 * @param opts Command line options.
 */
void smhcmd_ifindex(int opnum, char **opts)
{
     char *input, *output;     
     int op;
     int option_index = 0;
     
     static struct option long_options[] =
          {
               {"help", no_argument, 0, 'h'},
               {0, 0, 0, 0}
          };

     //Command-line option parser
     while((op = getopt_long( opnum, opts, "h", long_options, 
                              &option_index)) != -1){
          int this_option_optind = optind ? optind : 1;
          switch (op) {
          case 0:
               break;
          case 'h':
               usage();
               exit(EXIT_SUCCESS);
               break;
          case '?':
               fprintf(stderr,"Error: Unknown options.\n"
                       "Try `smhcmd --help' for more information.\n");
               exit(EXIT_FAILURE);
          default:
               abort ();
          }
     }
     if (optind + 2 == opnum){ 
          input = opts[optind++];
          output = opts[optind++];

          printf("Reading corpus file %s . . .\n", input);
          ListDB corpus = listdb_load_from_file(input);
          printf("Number of documents: %d\nVocabulary size: %d\n", corpus.size, corpus.dim);

          printf("Creating inverted file . . .\n");
          ListDB ifindex = ifindex_make_from_corpus(&corpus);

          printf("Saving inverted file into %s\n",output);
          listdb_save_to_file(output, &ifindex);
     } else {
          if (optind + 2 > opnum)
               fprintf(stderr, "Error: Missing arguments.\n"
                       "Try `smhcmd --help' for more information.\n");
          else
               fprintf(stderr, "Error: Unknown arguments.\n"
                       "Try `smhcmd --help' for more information.\n");
          exit(EXIT_FAILURE);
     }
}

/**
 * @brief Computes weights of the items in a database of lists
 *
 * @param opnum Number of command line options.
 * @param opts Command line options.
 */
void smhcmd_weights(int opnum, char **opts)
{
     char *weight_scheme = "idf";
     char *corpus_path, *ifindex_path, *output;     
     int op;
     int option_index = 0;
     
     static struct option long_options[] =
          {
               {"help", no_argument, 0, 'h'},
               {"weight", required_argument, 0, 'w'},
               {0, 0, 0, 0}
          };

     //Command-line option parser
     while((op = getopt_long( opnum, opts, "hw:", long_options, 
                              &option_index)) != -1){
          int this_option_optind = optind ? optind : 1;
          switch (op) {
          case 0:
               break;
          case 'h':
               usage();
               exit(EXIT_SUCCESS);
               break;
          case 'w':
               weight_scheme = optarg;
               break;
          case '?':
               fprintf(stderr,"Error: Unknown options.\n"
                       "Try `smhcmd --help' for more information.\n");
               exit(EXIT_FAILURE);
          default:
               abort ();
          }
     }
     if (optind + 3 == opnum){ 
          corpus_path= opts[optind++];
          ifindex_path = opts[optind++];
          output = opts[optind++];

          printf("Reading corpus file %s . . .\n", corpus_path);
          ListDB corpus = listdb_load_from_file(corpus_path);
          printf("Number of documents: %d\nVocabulary size: %d\n", corpus.size, corpus.dim);

          printf("Loading inverted file from file %s . . .\n", ifindex_path);
          ListDB ifindex = listdb_load_from_file(ifindex_path);
          double *weights;
          printf("Computing %s weights\n", weight_scheme);
          if ( strcmp(weight_scheme, "idf") == 0 ) {
               weights =  weights_from_corpus_and_ifindex(&corpus,
                                                          &ifindex,
                                                          weights_idf);
               printf("Saving weights into %s\n", output);
               weights_save_to_file(output, ifindex.size, weights);
          } else if ( strcmp(weight_scheme, "ids") == 0 ) {
               weights = weights_from_corpus_and_ifindex(&ifindex,
                                                         &corpus,
                                                         weights_ids);
               printf("Saving weights into %s\n", output);
               weights_save_to_file(output, corpus.size, weights);
          } else {
               printf ("Unrecognized weighting scheme %s.\n "
                       "Try `smhcmd --help' for more information.\n", 
                       weight_scheme);
               exit(EXIT_FAILURE);
          }
     } else {
          if (optind + 2 > opnum)
               fprintf(stderr, "Error: Missing arguments.\n"
                       "Try `smhcmd --help' for more information.\n");
          else
               fprintf(stderr, "Error: Unknown arguments.\n"
                       "Try `smhcmd --help' for more information.\n");
          exit(EXIT_FAILURE);
     }
}

/**
 * @brief Mines inverted file.
 *
 * @param opnum Number of command line options.
 * @param opts Command line options.
 */
void smhcmd_discover(int opnum, char **opts)
{
     uint mine_tuple_size = 3; // default tuple size
     uint mine_number_of_tuples = 255; // default number of tuples
     uint mine_table_size = 1048576; // default table size
     uint min_set_size = 3;
     uint cluster_tuple_size = 3; 
     uint cluster_number_of_tuples = 255;
     uint cluster_table_size = 1048576;
     double overlap = 0.7; 
     uint min_cluster_size = 3;
     char *input, *output, *weights_file = NULL,  *ifindex_file = NULL;
     
     int op;
     int option_index = 0;
     
     static struct option long_options[] =
          {
               {"help", no_argument, 0, 'h'},
               {"tuple_size", required_argument, 0, 'r'},
               {"number_of_tuples", required_argument, 0, 'l'},
               {"table_size", required_argument, 0, 't'},
               {"min_set_size", required_argument, 0, 's'},
               {"cluster_tuple_size", required_argument, 0, 'x'},
               {"cluster_number_of_tuples", required_argument, 0, 'y'},
               {"cluster_table_size", required_argument, 0, 'z'},
               {"min_cluster_size", required_argument, 0, 'c'},
               {"expand", required_argument, 0, 'e'},
               {"weights", required_argument, 0, 'w'},
               {0, 0, 0, 0}
          };

     //Command-line option parser
     while((op = getopt_long( opnum, opts, "hr:l:t:s:x:y:z:c:e:w:", long_options, 
                              &option_index)) != -1){
          int this_option_optind = optind ? optind : 1;
          switch (op)
          {
          case 0:
               break;
          case 'h':
               usage();
               exit(EXIT_SUCCESS);
               break;
          case 'r':
               mine_tuple_size = atoi(optarg);
               break;
          case 'l':
               mine_number_of_tuples = atoi(optarg);
               break;
          case 't':
               mine_table_size = (uint) pow(2, atoi(optarg));
               break;
          case 's':
               min_set_size = atoi(optarg);
               break;
          case 'x':
               cluster_tuple_size = atoi(optarg);
               break;
          case 'y':
               cluster_number_of_tuples = atoi(optarg);
               break;
          case 'z':
               cluster_table_size = atoi(optarg);
               break;
          case 'o':
               overlap = atof(optarg);
               break;
          case 'c':
               min_cluster_size = atoi(optarg);
               break;
          case 'e':
               ifindex_file = optarg;
               break;
          case 'w':
               weights_file = optarg;
               break;
          case '?':
               fprintf(stderr,"Error: Unknown options.\n"
                       "Try `smhcmd --help' for more information.\n");
               exit(EXIT_FAILURE);
          default:
               abort ();
          }
     }
     if (optind + 2 == opnum){
          input = opts[optind++];
          output = opts[optind++];

          printf("Reading sets from %s . . .\n", input);
          ListDB corpus = listdb_load_from_file(input);
          printf("Number of documents: %d\nVocabulary size: %d\n", corpus.size, corpus.dim);
          
          ListDB mined;
          if (weights_file != NULL) {
               printf("Loading weights . . . ");
               double *weights = weights_load_from_file(weights_file);
               printf("Mining . . . ");
               if (ifindex_file != NULL) {
                    ListDB ifindex = listdb_load_from_file(ifindex_file);
                    uint *maxfreq = mh_get_cumulative_frequency(&corpus, &ifindex);
                    ListDB expanded_corpus = mh_expand_listdb(&corpus, maxfreq);
                    mined = sampledmh_mine_weighted(&expanded_corpus,
                                                    mine_tuple_size,
                                                    mine_number_of_tuples,
                                                    mine_table_size,
                                                    weights);
               } else {
                    mined = sampledmh_mine_weighted(&corpus,
                                                    mine_tuple_size,
                                                    mine_number_of_tuples,
                                                    mine_table_size,
                                                    weights);
               }
          } else {
               printf("Mining . . . ");
               if (ifindex_file != NULL) {
                    ListDB ifindex = listdb_load_from_file(ifindex_file);
                    uint *maxfreq = mh_get_cumulative_frequency(&corpus, &ifindex);
                    ListDB expanded_corpus = mh_expand_listdb(&corpus, maxfreq);
                    mined = sampledmh_mine(&expanded_corpus,
                                           mine_tuple_size,
                                           mine_number_of_tuples,
                                           mine_table_size);
               } else {
                    mined = sampledmh_mine(&corpus,
                                           mine_tuple_size,
                                           mine_number_of_tuples,
                                           mine_table_size);
               }
          }
          
          printf("Sorting sets by size and deleting the smallest ones . . .\n");
          listdb_sort_by_size_back(&mined);
          listdb_delete_smallest(&mined, min_set_size);
          printf("Number of mined sets: %d\nDimensionality: %d\n", mined.size, mined.dim);

          printf("Clustering mined sets . . .\n");
          ListDB models = mhlink_cluster(&mined,
                                         cluster_tuple_size,
                                         cluster_number_of_tuples,
                                         cluster_table_size,
                                         list_overlap,
                                         overlap,
                                         min_cluster_size);
          
          printf("Saving models in %s\n", output);
          listdb_save_to_file(output, &models);
     } else {
          if (optind + 2 > opnum)
               fprintf(stderr, "Error: Missing arguments.\n"
                       "Try `smhcmd --help' for more information.\n");
          else
               fprintf(stderr, "Error: Unknown arguments.\n"
                       "Try `smhcmd --help' for more information.\n");
          exit(EXIT_FAILURE);
     }
}

/**
 * ======================================================
 * @brief Main function
 * ======================================================
 */
int main(int argc, char **argv)
{
     
     //initialize Mersenne Twister random number generator
     unsigned long long init[4]={0x12345ULL, 0x23456ULL, 0x34567ULL, 0x45678ULL}, length=4;
     init_by_array64(init, length);

     if ( argc > 1 ){
          if ( strcmp(argv[1], "ifindex") == 0 )
               smhcmd_ifindex(argc - 1, &argv[1]);
          else if ( strcmp(argv[1], "weights") == 0 )
               smhcmd_weights(argc - 1, &argv[1]);
          else if ( strcmp(argv[1], "discover") == 0 )
               smhcmd_discover(argc - 1, &argv[1]);
          else if ( strcmp(argv[1], "--help") == 0 || 
                    strcmp(argv[1], "-h") == 0 ){
               usage();
               exit(EXIT_SUCCESS);
          } else {
               printf ("Unrecognized option %s.\n "
                       "Try `smhcmd --help' for more information.\n", 
                       argv[1]);
               exit(EXIT_FAILURE);
          }
     } else {
          printf ("Missing arguments.\n"
                  "Try `smhcmd --help' for more information.\n");
          exit(EXIT_FAILURE);
     }
     
     return 1;
}
