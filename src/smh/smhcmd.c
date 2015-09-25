/**
 * @file mhclus.c 
 * @author Gibran Fuentes Pineda <gibranfp@turing.iimas.unam.mx>
 * @date 2015
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
#include <math.h>
#include <time.h>
#include <getopt.h>
#include <inttypes.h>
#include "ifindex.h"
#include "sampledmh.h"
#include "mhlink.h"

enum WeightScheme {TF, LOGTF, BINTF, IDF, TFIDF};

/**
 * @brief Prints help in screen.
 */
void usage(void)
{
     printf("usage: smhcmd ifindex [OPTIONS]... [CORPUS_FILE] [INVERTED_FILE]\n"
            "       smhcmd mine [OPTIONS]... [INPUT_FILE] [OUTPUT_FILE]\n"
            "       smhcmd prune [OPTIONS]... [INVERTED_FILE] [INPUT_FILE] [OUTPUT_FILE]\n"
            "       smhcmd cluster [OPTIONS]... [INPUT_FILE] [OUTPUT_FILE]\n"
            "Mines inverted file structures, prunes and clusters mined sets,"
            "and writes the output to a file .\n\n"
            "General options:\n"
            "   --help\t\tPrints this help\n"
            "ifindex options:\n"
            "   -w, --weight[=binary]\tWeighting scheme to use."
            "mine options:\n"
            "   -s, --size[=20(2^20 = 1048576)]\tNumber of buckets in hash"
            "table (power of 2)\n"
            "   -r, --hashes[=4]\tNumber of hashes per tuple\n"
            "   -l, --tuples[=500]\tNumber of tuples\n"
            "prune options:\n"
            "   -s, --stop[=3]\tMinimum set size (stop > 0)\n"
            "   -d, --dochits[=3]\tMinimum document hits per set (dochits > 0)\n"
            "   -o, --overlap[=0.7]\tOverlap threshold (0 <= overlap <= 1)\n"
            "   -c, --coocc[=0.7]\tWord cooccurrence in documents " 
            " containing set (0 <= coocc <= 1)\n"
            "cluster options:\n"
            "   -s, --size[=1048576]\tNumber of buckets in hash"
            "table (power of 2)\n"
            "   -r, --tnum[=3]\tNumber of hashes per tuple\n"
            "   -l, --tuples[=255]\tNumber of tuples\n"
            "   -o, --overlap[=0.7]\tOverlap threshold\n");
}

/**
 * @brief Creates inverted file from corpus file.
 *
 * @param opnum Number of command line options.
 * @param opts Command line options.
 */
void smhcmd_ifindex(int opnum, char **opts)
{
     enum WeightScheme weight_scheme = TF;
     char *input, *output;     
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
               weight_scheme = atoi(optarg);
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
          printf("\tNumber of documents: %d\tVocabulary size: %d\n", corpus.size, corpus.dim);

          printf("Creating inverted file . . .\n");
          ListDB ifindex = ifindex_make_from_corpus(&corpus);
          if (weight_scheme != TF){
               printf("Computing weights . . .\n");
               double (*weighting)(uint *, uint *, uint *);
               switch (weight_scheme){
               case LOGTF:
                    weighting = &logtf;
                    break;
               case BINTF:
                    weighting = &bintf;
                    break;
               case IDF:
                    weighting = &idf;
                    break;
               case TFIDF:
                    weighting = &tfidf;
                    break;
               case '?':
                    fprintf(stderr,"Error: Unknown type of weighting.\n"
                            "Try `smhcmd ifindex --help' for more information.\n");
                    exit(EXIT_FAILURE);
               default:
                    abort ();
               }
               ifindex_weight(&ifindex, &corpus, weighting);
          }
          
          printf("Saving inverted file into %s\n",output);
          listdb_save_to_file(output, &ifindex);
     } else {
          if (optind + 2 > opnum)
               fprintf(stderr, "Error: Missing argumets.\n"
                       "Try `smhcmd --help' for more information.\n");
          else
               fprintf(stderr, "Error: Unknown argumets.\n"
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
void smhcmd_mine(int opnum, char **opts)
{
     uint tuple_size = 4; // default tuple size
     uint number_of_tuples = 500; // default number of tuples
     uint table_size = 1048576; // default table size
     char *input, *output;
     
     int op;
     int option_index = 0;
     
     static struct option long_options[] =
          {
               {"help", no_argument, 0, 'h'},
               {"tuple_size", required_argument, 0, 'r'},
               {"number_of_tuples", required_argument, 0, 'l'},
               {"table_size", required_argument, 0, 't'},
               {0, 0, 0, 0}
          };

     //Command-line option parser
     while((op = getopt_long( opnum, opts, "hr:l:t:", long_options, 
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
               tuple_size = atoi(optarg);
               break;
          case 'l':
               number_of_tuples = atoi(optarg);
               break;
          case 't':
               table_size = (uint) pow(2, atoi(optarg));
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
          printf("\tNumber of documents: %d\tVocabulary size: %d\n", corpus.size, corpus.dim);

          printf("Mining . . .\n");
          ListDB mined = sampledmh_mine(&corpus, tuple_size, number_of_tuples, table_size);
          printf("\tNumber of mined sets: %d\tDimensionality: %d\n", mined.size, mined.dim);

          printf("Sorting items by size . . .\n");
          listdb_sort_by_size_back(&mined);
          listdb_delete_smallest(&mined,5);
          
          printf("Saving file in %s . . .\n", output);
          listdb_save_to_file(output, &mined);
     } else {
          if (optind + 2 > opnum)
               fprintf(stderr, "Error: Missing argumets.\n"
                       "Try `smhcmd --help' for more information.\n");
          else
               fprintf(stderr, "Error: Unknown argumets.\n"
                       "Try `smhcmd --help' for more information.\n");
          exit(EXIT_FAILURE);
     }
}

/**
 *
 * @brief Prunes mined co-occurring word sets.
 *
 * @param opnum Number of command line options.
 * @param opts Command line options.
 *
 */
void smhcmd_prune(int opnum, char **opts)
{
     char *inv_file, *mined_file, *pruned_file;
     uint stop = 3;
     uint dochits = 3;
     double ovr = 0.7;
     double coocc = 0.7;
     int op;
     int option_index = 0;
     
     static struct option long_options[] =
          {
               {"help", no_argument, 0, 'h'},
               {"stop", required_argument, 0, 's'},
               {"dochits", required_argument, 0, 'd'},
               {"overlap", required_argument, 0, 'o'},
               {"coocc", required_argument, 0, 'c'},
               {0, 0, 0, 0}
          };

     //Command-line ption parser
     while((op = getopt_long( opnum, opts, "hs:d:o:c:", long_options,
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
          case 's':
               stop = atoi(optarg);
               if (stop <= 0)
                    stop = 1;
               break;
          case 'd':
               dochits = atoi(optarg);
               if (dochits <= 0)
                    dochits = 1;
               break;
          case 'o':
               ovr = atof(optarg);
               if (ovr < 0)
                    ovr = 0.0;
               if (ovr > 1)
                    ovr = 1.0;
               break;
          case 'c':
               coocc = atof(optarg);
               if (coocc < 0)
                    coocc = 0.0;
               if (ovr > 1)
                    coocc = 1.0;
               break;
          case '?':
               printf("Try `smhcmd --help' for more information.\n");
               exit(EXIT_FAILURE);
               break;
          default:
               abort ();
          }
     }
     if (optind + 3 == opnum){
          inv_file = opts[optind++];
          mined_file = opts[optind++];
          pruned_file = opts[optind++];

          printf("Reading inverted file %s . . .\n", inv_file);
          ListDB ifindex = listdb_load_from_file(inv_file);

          printf("Reading mined sets from %s . . .\n", mined_file);
          ListDB mined = listdb_load_from_file(mined_file);

          printf("Pruning sets . . .\n");
          sampledmh_prune(&ifindex, &mined, stop, dochits, ovr, coocc);
          printf("\tNumber of sets after pruning: %d\tDimensionality: %d\n", mined.size, mined.dim);

          printf("Saving file in %s . . .\n", pruned_file);
          listdb_save_to_file(pruned_file, &mined);
     } else {
          if (optind + 3 > opnum)
               fprintf(stderr, "Error: Missing argumets.\n"
                       "Try `smhcmd --help' for more information.\n");
          else
               fprintf(stderr, "Error: Unknown argumets.\n"
                       "Try `smhcmd --help' for more information.\n");
          exit(EXIT_FAILURE);
     }
}

/**
 *
 * @brief Clusters mined co-occurring word sets.
 *
 * @param opnum Number of command line options.
 * @param opts Command line options.
 *
 */
void smhcmd_cluster(int opnum, char **opts)
{
     char *mined_file, *clusters_file, *models_file;
     uint tuple_size = 3;//Default tuple size
     uint number_of_tuples = 255;//Default tuple number
     uint table_size = 1048576;//Defualt table size
     double ovr = 0.7;//overlap
     int op;
     int option_index = 0;
     
     static struct option long_options[] =
          {
               {"help", no_argument, 0, 'h'},
               {"hashes", required_argument, 0, 'r'},
               {"tuples", required_argument, 0, 'l'},
               {"size", required_argument, 0, 's'},
               {"overlap", required_argument, 0, 'o'},
               {0, 0, 0, 0}
          };

     //Command-line ption parser
     while((op = getopt_long( opnum, opts, "hs:l:r:o:", long_options,
                              &option_index)) != -1){
          int this_option_optind = optind ? optind : 1;
          switch (op)
          {
          case 'h':
               usage();
               exit(EXIT_SUCCESS);
               break;
          case 'r':
               tuple_size = atoi(optarg);
               break;
          case 'l':
               number_of_tuples = atoi(optarg);
               break;
          case 's':
               table_size = atoi(optarg);
               break;
          case 'o':
               ovr = atof(optarg);
               break;
          case '?':
               fprintf(stderr, "Error: Try `smhcmd --help' for more "
                       "information.\n");
               exit(EXIT_FAILURE);
               break;
          default:
               abort ();
          }
     }
     if (optind + 3 == opnum){
          mined_file= opts[optind++];
          clusters_file = opts[optind++];	  
          models_file = opts[optind++];	  

          printf("Reading mined sets from %s . . .\n", mined_file);
          ListDB mined = listdb_load_from_file(mined_file);


          printf("Clustering sets . . .\n");
          ListDB clusters = mhlink_cluster(&mined, table_size, number_of_tuples, tuple_size, list_overlap, ovr);

          printf("Deleting smallest clusters . . .\n");
          listdb_delete_smallest(&clusters, 1);
          
          printf("Saving clusters in %s\n", clusters_file);
          listdb_save_to_file(clusters_file, &clusters);

          printf("Creating models . . . \n");
          ListDB models = mhlink_make_model(&mined, &clusters);

          printf("Saving models in %s\n", models_file);
          listdb_save_to_file(models_file, &models);
     }
     else{
          if (optind + 2 > opnum)
               fprintf(stderr, "Error: Missing argumets.\n"
                       "Try `smhcmd --help' for more information.\n");
          else
               fprintf(stderr, "Error: Unknown argumets.\n"
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
     // initialize built-in random number generator
     srand((unsigned int) 123456);
     
     // initialize Mersenne Twister random number generator
    unsigned long long init[4]={0x12345ULL, 0x23456ULL, 0x34567ULL, 0x45678ULL}, length=4;
    init_by_array64(init, length);

     if ( argc > 1 ){
          if ( strcmp(argv[1], "mine") == 0 )
               smhcmd_mine(argc - 1, &argv[1]);
          else if ( strcmp(argv[1], "prune") == 0 )
               smhcmd_prune(argc - 1, &argv[1]);
          else if ( strcmp(argv[1], "cluster") == 0 )
               smhcmd_cluster(argc - 1, &argv[1]);
          else if ( strcmp(argv[1], "ifindex") == 0 )
               smhcmd_ifindex(argc - 1, &argv[1]);
          else if ( strcmp(argv[1], "--help") == 0 || 
                    strcmp(argv[1], "-h") == 0 ){
               usage();
               exit(EXIT_SUCCESS);
          }
          else {
               printf ("Unrecognized option %s.\n "
                       "Try `smhcmd --help' for more information.\n", 
                       argv[1]);
               exit(EXIT_FAILURE);
          }
     }
     else {
          printf ("Missing arguments.\n"
                  "Try `smhcmd --help' for more information.\n");
          exit(EXIT_FAILURE);
     }
     
     return 1;
}
