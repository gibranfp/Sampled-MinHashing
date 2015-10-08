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

#include "types.h"

double termfreq(uint, uint, uint, uint, uint, uint);
double logtf(uint, uint, uint, uint, uint, uint);
double bintf(uint, uint, uint, uint, uint, uint);
double idf(uint, uint, uint, uint, uint, uint);
double itf(uint, uint, uint, uint, uint, uint);
double ids(uint, uint, uint, uint, uint, uint);
double tfidf(uint, uint, uint, uint, uint, uint);
double logtfidf(uint, uint, uint, uint, uint, uint);
double itfidf(uint, uint, uint, uint, uint, uint);
double logitfidf(uint, uint, uint, uint, uint, uint);
double tfids(uint, uint, uint, uint, uint, uint);
double logtfids(uint, uint, uint, uint, uint, uint);
double itfidfids(uint, uint, uint, uint, uint, uint);
double logitfidfids(uint, uint, uint, uint, uint, uint);
double tfdr(uint, uint, uint, uint, uint, uint);
double logtfdr(uint, uint, uint, uint, uint, uint);
double drtf(uint, uint, uint, uint, uint, uint);
double drlogtf(uint, uint, uint, uint, uint, uint);
uint intweight(double);
#endif
