/**
 * @file weights.h 
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
 * @brief Declarations for weighting functions.
 */
#ifndef WEIGHTS_H
#define WEIGHTS_H

#include "listdb.h"
#include "types.h"

double weights_termfreq(uint, uint, uint, uint, uint, uint);
double weights_logtf(uint, uint, uint, uint, uint, uint);
double weights_bintf(uint, uint, uint, uint, uint, uint);
double weights_idf(uint, uint, uint, uint, uint, uint);
double weights_itf(uint, uint, uint, uint, uint, uint);
double weights_ids(uint, uint, uint, uint, uint, uint);
double weights_tfidf(uint, uint, uint, uint, uint, uint);
double weights_logtfidf(uint, uint, uint, uint, uint, uint);
double weights_itfidf(uint, uint, uint, uint, uint, uint);
double weights_logitfidf(uint, uint, uint, uint, uint, uint);
double weights_tfids(uint, uint, uint, uint, uint, uint);
double weights_logtfids(uint, uint, uint, uint, uint, uint);
double weights_itfidfids(uint, uint, uint, uint, uint, uint);
double weights_logitfidfids(uint, uint, uint, uint, uint, uint);
double weights_tfdr(uint, uint, uint, uint, uint, uint);
double weights_logtfdr(uint, uint, uint, uint, uint, uint);
double weights_drtf(uint, uint, uint, uint, uint, uint);
double weights_drlogtf(uint, uint, uint, uint, uint, uint);
uint weights_intweight(double);
double *weights_from_corpus_and_ifindex(ListDB *, ListDB *, double (*)(uint,uint,uint,uint,uint,uint));
double *weights_load_from_file(char *);
void weights_save_to_file(char *, uint, double *);
#endif
