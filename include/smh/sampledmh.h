/**
 * @file sampledmh.h
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
 * @brief Declaration of functios for Sampled Min-Hashing (SMH)
 */
#ifndef SAMPLEDMH_H
#define  SAMPLEDMH_H

#include "minhash.h"

void sampledmh_get_coitems(ListDB *, HashTable *, uint);
ListDB sampledmh_expand_frequencies(ListDB *, ListDB *);
ListDB sampledmh_expand_frequencies_and_weights(ListDB *, ListDB *, double *, double *);
ListDB sampledmh_mine(ListDB *, uint, uint, uint, uint);
ListDB sampledmh_mine_weighted(ListDB *, uint, uint, uint, double *, uint);
void sampledmh_prune(ListDB *, ListDB *, uint, uint, double, double);
#endif
