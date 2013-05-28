/**
 * @file make_ifs.c 
 * @authors Gibran Fuentes Pineda <gibranfp@turing.iimas.unam.mx>
 *          Ivan Vladimir Meza Ruiz <ivanvladimir@turing.iimas.unam.mx>
 * @version 0.1
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
#include <math.h>
#include <inttypes.h>
#include "types.h"
#include "ifutils.h"
#include "weights.h"

/**
 * @brief Term frequency weighting
 */
double termfreq(const void *tf, const void *df, const void *corpsize)
{
     return *(uint *)tf;
}

/**
 * @brief Logarithmic term frequency
 */
double logtf(const void *tf, const void *df, const void *corpsize)
{
     return log(*(uint *)tf + 1);
}

/**
 * @brief Binary term frequency
 */
double bintf(const void *tf, const void *df, const void *corpsize)
{
     return 1.0;
}

/**
 * @brief Inverse document frequency
 */
double idf(const void *tf, const void *df, const void *corpsize)
{
     return log (*(uint *)corpsize / *(uint *)df);
}

/**
 * @brief Term frequency inverse document frequency
 */
double tfidf(const void *tf, const void *df, const void *corpsize)
{
     return termfreq(tf, df, corpsize) * idf(tf, df, corpsize);
}

double tfidf_weight(uint tf, uint df, uint corpsize)
{
     return tf * log (corpsize / df);
}

/**
 * @brief Term frequency inverse document frequency
 */
uint intweight(double weight)
{
return round(weight * 100000000L);
}
