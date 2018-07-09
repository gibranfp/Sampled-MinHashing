/**
 * @file mhlink.h
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
 * @brief Declaration of functions for performing single-link clustering
 *        accelerated by Min-Hashing
 */
#ifndef MHLINK_H
#define MHLINK_H

#include "minhash.h"

ListDB mhlink_make_model(ListDB *, ListDB *);
void mhlink_add_neighbors(ListDB *, ListDB *, uint , List *, uint *, uint *, 
			  double (*)(List *, List *), double);
ListDB mhlink_cluster(ListDB *, uint, uint, uint, double (*)(List *, List *), double, uint);
ListDB mhlink_cluster_weighted(ListDB *, uint, uint, uint, double *,
                               double (*)(List *, List *), double, uint);
#endif
