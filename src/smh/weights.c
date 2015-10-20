/**
 * @file weights.c 
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
 * @brief This program makes the inverted file structure of a given
 *        corpus using different weighting schemes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include "weights.h"

/**
 * @brief Term frequency weighting
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_termfreq(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return (double) tf;
}

/**
 * @brief Logarithmic term frequency
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_logtf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return log((double)tf + 1.0);
}

/**
 * @brief Binary term frequency
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_bintf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return 0.000000001;
}

/**
 * @brief Inverse document frequency
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_idf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     printf("Pase22\n");
     return log ((double) corpsize / (double) df);
               printf("Pase3e\n");
}

/**
 * @brief Inverse term frequency
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_itf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return log((double) docsize / (double)tf);
}

/**
 * @brief Inverse document size
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_ids(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return log ((double)vocsize / (double)docterms);
}

/**
 * @brief Term frequency inverse document frequency
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_tfidf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     double idf = weights_idf(tf, df, corpsize, docsize, docterms, vocsize);
     
     return (double)tf * idf;
}

/**
 * @brief Logaritmic term frequency inverse document frequency
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_logtfidf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     double logtf = weights_logtf(tf, df, corpsize, docsize, docterms, vocsize);
     double idf = weights_idf(tf, df, corpsize, docsize, docterms, vocsize);
     
     return  logtf * idf;
}

/**
 * @brief Inverse term frequency inverse document frequency
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_itfidf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     double itf = weights_itf(tf, df, corpsize, docsize, docterms, vocsize);
     double idf = weights_idf(tf, df, corpsize, docsize, docterms, vocsize);
     
     return  itf * idf;
}

/**
 * @brief Logaritmic inverse term frequency inverse document frequency
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_logitfidf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     double itf = weights_itf(tf, df, corpsize, docsize, docterms, vocsize);
     double idf = weights_idf(tf, df, corpsize, docsize, docterms, vocsize);

     return log(itf * idf);
}

/**
 * @brief Term frequency inverse document size
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_tfids(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     double ids = weights_ids(tf, df, corpsize, docsize, docterms, vocsize);
     
     return tf * ids;
}

/**
 * @brief Logaritmic term frequency inverse document size
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_logtfids(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     double logtf = weights_logtf(tf, df, docsize, docterms, corpsize, vocsize);
     double ids = weights_ids(tf, df, corpsize, docsize, docterms, vocsize);
     
     return logtf * ids;
}

/**
 * @brief Inverse term frequency inverse document frequency
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_itfidfids(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     double itf = weights_itf(tf, df, corpsize, docsize, docterms, vocsize);
     double idf = weights_idf(tf, df, corpsize, docsize, docterms, vocsize);
     double ids = weights_ids(tf, df, corpsize, docsize, docterms, vocsize);
     
     return itf * idf * ids;
}

/**
 * @brief Logaritmic inverse term frequency inverse document frequency
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_logitfidfids(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     double itf = weights_itf(tf, df, corpsize, docsize, docterms, vocsize);
     double idf = weights_idf(tf, df, corpsize, docsize, docterms, vocsize);
     double ids = weights_ids(tf, df, corpsize, docsize, docterms, vocsize);
     
     return log(itf * idf * ids);
}

/**
 * @brief Ratio term frequency size
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_tfdr(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return (double) tf / (double) docsize;
}

/**
 * @brief Logaritmic term frequency inverse document frequency
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_logtfdr(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     double logtf = weights_logtf(tf, df, corpsize, docsize, docterms, vocsize);
     
     return logtf / (double) docsize;
}

/**
 * @brief Ratio term frequency size
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_drtf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return (double) docsize / (double) tf;
}

/**
 * @brief Logaritmic term frequency inverse document frequency
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
double weights_drlogtf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     double logtf = weights_logtf(tf, df, corpsize, docsize, docterms, vocsize);
     
     return (double) docsize / logtf;
}

/**
 * @brief Term frequency inverse document frequency
 *
 * @param tf Term frequency
 * @param df Document frequency
 * @param docsize Size of documents
 * @param docterms Number of different terms in a document
 * @param corpsize Size of the corpus
 * @param vocsize Size of the vocabulary
 *
 * @return weight of the term
 */
uint weights_intweight(double weight)
{
     return round(weight * 100000000L);
}

/**
 * @brief Computes weights of the items in an inverted file
 *
 * @param ifindex Inverted file
 *
 * @return Array with the weight of each item
 */
double *weights_from_corpus_and_ifindex(ListDB *corpus, ListDB *ifindex,
                                        double (*wg)(uint,uint,uint,uint,uint,uint))
{
     double *weights = (double *) malloc(ifindex->size * sizeof(double));
     
     uint i;
     for (i = 0; i < ifindex->size; i++)
          weights[i] = wg(0, ifindex->lists[i].size, 0, 0, corpus->size, ifindex->size);
     
     return weights;
}

/**
 * @brief Loads the weights of all the items from a file
 *        Format: 
 *             Weight_item1
 *             Weight_item2
 *                 ...
 *             Weight_itemN
 *
 * @param filename File containing the weights
 *
 * @return Array with the weight of each item
 */
double *weights_load_from_file(char *filename)
{
     FILE *file;
     if (!(file = fopen(filename,"r"))) {
          fprintf(stderr,"Error: Could not open file %s\n", filename);
          exit(EXIT_FAILURE);
     }

     // checking the number of lists in the file
     size_t len = 0;
     ssize_t read;
     char *line = NULL;

     uint number_of_items = 0;
     while ((read = getline(&line, &len, file)) != -1)
          number_of_items++;
     rewind(file);

     // reading lists
     double *weights = (double *) malloc(number_of_items * sizeof(double));

     uint i;
     for (i = 0; i < number_of_items; i ++) 
          fscanf(file,"%lf", &weights[i]);
     
     if (fclose(file)) {
          fprintf(stderr,"Error: Could not close file %s\n", filename);
          exit(EXIT_FAILURE);
     }
     
     return weights;
}

/**
 * @brief Saves the weights of all the items to a file
 *        Format: 
 *             Weight_item1
 *             Weight_item2
 *                 ...
 *             Weight_itemN
 *
 * @param filename File where the weights are to be saved
 * @return Array with the weight of each item
 *
 */
void weights_save_to_file(char *filename, uint number_of_items, double *weights)

{
     FILE *file;
     
     if (!(file = fopen(filename,"w"))) {
          fprintf(stderr,"Error: Could not create file %s\n", filename);
          exit(EXIT_FAILURE);
     }

     uint i;
     for (i = 0; i < number_of_items; i++)
          fprintf(file," %lf\n", weights[i]);
     
     if (fclose(file)) {
          fprintf(stderr,"Error: Could not close file %s\n", filename);
          exit(EXIT_FAILURE);
     }
}
