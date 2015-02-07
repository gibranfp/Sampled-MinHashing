/**
 * @file mhlink.h
 * @author Gibran Fuentes Pineda <gibranfp@turing.iimas.unam.mx>
 * @date 2014
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
 * @brief Declaration of functios to performing single-link clustering
 *        accelerated by Min-Hashing
 */
#ifndef MHLINK_H
#define MHLINK_H

#include "minhash.h"

void mhlink_make_model(ListDB *listdb, ListDB *clusters, ListDB *models);
void mhlink_add_neighbors(ListDB *listdb, ListDB *clusters, uint listid, List *items, uint *checked, 
						   uint *clus_table, double (*sim)(List *, List *), double thres);
void mhlink_cluster(ListDB *listdb, ListDB *clusters, uint table_size, uint number_of_tuples,
					uint tuple_size, double (*sim)(List *, List *), double thres);
#endif
