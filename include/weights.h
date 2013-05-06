/**
 * @file weights.h 
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
#ifndef WEIGHTS_H
#define WEIGHTS_H

#include "types.h"

double tf(uint tf, uint df, uint corpsize);
double logtf(uint tf, uint df, uint corpsize);
double bintf(uint tf, uint df,uint corpsize);
double idf(uint tf, uint df, uint corpsize);
double tfidf(uint tf, uint df, uint corpsize);
uint intweight(double weight);
#endif
