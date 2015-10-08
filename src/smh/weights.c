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
#include <math.h>
#include <inttypes.h>
#include "weights.h"

/**
 * @brief Term frequency weighting
 */
double termfreq(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return (double) tf;
}

/**
 * @brief Logarithmic term frequency
 */
double logtf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return log((double)tf + 1.0);
}

/**
 * @brief Binary term frequency
 */
double bintf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
	return 0.000000001;
}

/**
 * @brief Inverse document frequency
 */
double idf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return log ((double)corpsize / (double)df);
}

/**
 * @brief Inverse term frequency
 */
double itf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return log((double) docsize / (double)tf);
}

/**
 * @brief Inverse document size
 */
double ids(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return log ((double)vocsize / (double)docterms);
}

/**
 * @brief Term frequency inverse document frequency
 */
double tfidf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return (double)tf * idf(tf, df, corpsize, docsize, docterms, vocsize);
}

/**
 * @brief Logaritmic term frequency inverse document frequency
 */
double logtfidf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return logtf(tf, df, corpsize, docsize, docterms, vocsize) * idf(tf, df, corpsize, docsize, docterms, vocsize);
}

/**
 * @brief Inverse term frequency inverse document frequency
 */
double itfidf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return itf(tf, df, corpsize, docsize, docterms, vocsize) * idf(tf, df, corpsize, docsize, docterms, vocsize);
}

/**
 * @brief Logaritmic inverse term frequency inverse document frequency
 */
double logitfidf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return log(itf(tf, df, corpsize, docsize, docterms, vocsize) * idf(tf, df, corpsize, docsize, docterms, vocsize));
}

/**
 * @brief Term frequency inverse document size
 */
double tfids(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return tf * ids(tf, df, corpsize, docsize, docterms, vocsize);
}

/**
 * @brief Logaritmic term frequency inverse document size
 */
double logtfids(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return logtf(tf, df, docsize, docterms, corpsize, vocsize) * ids(tf, df, corpsize, docsize, docterms, vocsize);
}

/**
 * @brief Inverse term frequency inverse document frequency
 */
double itfidfids(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return itf(tf, df, corpsize, docsize, docterms, vocsize) * idf(tf, df, corpsize, docsize, docterms, vocsize) * ids(tf, df, corpsize, docsize, docterms, vocsize);
}

/**
 * @brief Logaritmic inverse term frequency inverse document frequency
 */
double logitfidfids(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return log(itf(tf, df, corpsize, docsize, docterms, vocsize) * idf(tf, df, corpsize, docsize, docterms, vocsize) * ids(tf, df, corpsize, docsize, docterms, vocsize));
}

/**
 * @brief Ratio term frequency size
 */
double tfdr(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return (double) tf / (double) docsize;
}

/**
 * @brief Logaritmic term frequency inverse document frequency
 */
double logtfdr(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return logtf(tf, df, docsize, docterms, corpsize, vocsize) / (double) docsize;
}

/**
 * @brief Ratio term frequency size
 */
double drtf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return (double) docsize / (double) tf;
}

/**
 * @brief Logaritmic term frequency inverse document frequency
 */
double drlogtf(uint tf, uint df, uint docsize, uint docterms, uint corpsize, uint vocsize)
{
     return (double) docsize / logtf(tf, df, docsize, docterms, corpsize, vocsize);
}

/**
 * @brief Term frequency inverse document frequency
 */
uint intweight(double weight)
{
	return round(weight * 100000000L);
}
