/**
 * @file make_ifs.c 
 * @author Gibran Fuentes Pineda <gibranfp@turing.iimas.unam.mx>
 * @date 2013
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
 * @brief This program makes the inverted file structure of a given
 *        corpus using different weighting schemes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <getopt.h>
#include <float.h>
#include <inttypes.h>
#include "types.h"
#include "ifutils.h"
#include "weights.h"

/**
 * @brief Prints help in screen.
 */
void usage(void)
{
	printf("usage: make_ifs [OPTIONS] ... [CORPUS_FILE] "
		   "[VOCABULARY_FILE] [OUTPUT_FILE]\n"
		   "Makes the inverted file structure of a given corpus.\n"
		   "Options:\n"
		   "   --help\t\tPrints this help\n"
		   "   -w, --weight[=0]\tWeighting scheme: 0 - tf-idf\n"
		   "                   \t\t\t  1 - tf\n"
		   "                   \t\t\t  2 - binary\n");
}

/**
 *
 * @brief Makes inverted file from a given corpus
 *
 * @param corpus Corpus
 * @param termfreq Frequency of each term in the document
 * @param doc_card Number of terms in each document
 * @param corpus_size Number of documents in the corpus
 */
void make_ifs(uint **corpus, uint **termfreq, uint *doccard, uint corpsize, 
			  uint *corpfreq, uint *docfreq, uint vocsize,
			  uint ***ifs, uint ***weight, uint **ifs_card,
			  double (*wg)(const void*,const void*,const void*))
{
	uint i, j;
	uint tid;
	double wval;
	uint **db = (uint **) malloc(vocsize * sizeof(uint *));
	uint **tw = (uint **) malloc(vocsize * sizeof(uint *));
	uint *tc = (uint *) calloc(vocsize, sizeof(uint));

	for (i = 0; i < vocsize; i++) {
		db[i] = (uint *) malloc(docfreq[i] * sizeof(uint));
		tw[i] = (uint *) malloc(docfreq[i] * sizeof(uint));
	}

	for (i = 0; i < corpsize; i++) {
		for (j = 0; j < doccard[i]; j++) {
			tid = corpus[i][j];
			db[tid][tc[tid]] = i;
			wval = (double) termfreq[i][j] * log((double) corpsize / (double) docfreq[tid]);
			tw[tid][tc[tid]] = intweight(wval);
			if (tw[tid][tc[tid]] == 0) {
				tw[tid][tc[tid]] = 1;
			}
			tc[tid]++;
		}
	}
     
	*ifs = db;
	*weight = tw;
	*ifs_card = tc;
}

/**
 * ======================================================
 * @brief Main function
 * ======================================================
 */
int main(int argc, char **argv)
{
	uint **ifs;
	uint **weight;
	uint *ifs_card;
	uint **corpus;
	uint **termfreq;
	uint *doccard;
	uint corpsize;
	uint *termid;
	uint *corpfreq;
	uint *docfreq;
	uint vocsize;
	char **vocab;
	char *corpus_file, *vocab_file, *output_file;
	uint wtype;
	//Variables for getopt
	int op;
	int option_index = 0;
     
	static struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"weight", required_argument, 0, 'w'},
		{0, 0, 0, 0}
	};
     
	//Command-line option parser
	while((op = getopt_long( argc, argv, "hw:", long_options, &option_index)) != -1) {
		int this_option_optind = optind ? optind : 1;
		switch (op) {
		case 0:
			break;
		case 'h':
			usage();
			exit(EXIT_SUCCESS);
			break;
		case 'w':
			wtype = atoi(optarg);
			break;
		case '?':
			fprintf(stderr,"Error: Unknown options.\n"
					"Try 'make_ifs --help' for more information."
					"\n");
			exit(EXIT_FAILURE);
		default:
			abort ();
		}
	}

	if (optind + 3 == argc) {
		corpus_file = argv[optind++];
		vocab_file = argv[optind++];
		output_file = argv[optind++];
		read_corpus(corpus_file, &corpus, &termfreq, &doccard, 
					&corpsize);
		read_vocab(vocab_file, &vocab, &termid, &corpfreq, 
				   &docfreq, &vocsize);
		make_ifs(corpus, termfreq, doccard, corpsize, 
				 corpfreq, docfreq, vocsize, &ifs, &weight, &ifs_card, tfidf);
		setwdb_write(output_file, ifs, weight, ifs_card, vocsize, 
					 corpsize);
	} else {
		if (optind + 3 > argc) {
			fprintf(stderr, "Error: Missing arguments.\n"
					"Try `make_ifs --help' for more information."
					"\n");
		} else {
			fprintf(stderr, "Error: Unknown arguments.\n"
					"Try `make_ifs --help' for more information."
					"\n");
		}
		exit(EXIT_FAILURE);
	}
}
