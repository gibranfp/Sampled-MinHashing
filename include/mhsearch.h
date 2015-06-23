/**
 * @file mhsearch.h
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
 * @brief Declaration of structures and functions for searching based on Min-Hashing
 */
#ifndef MHSEARCH_H
#define MHSEARCH_H

#include "minhash.h"

HashIndex mhsearch_build(ListDB *listdb, uint number_of_tuples, uint tuple_size, uint table_size);
List mhsearch_query(List *query, HashIndex *hash_index);
void mhsearch_sort_custom(List *query, List *neighbors, ListDB *listdb, double (*func)(List *, List *));
ListDB mhsearch_query_multi(ListDB *queries, HashIndex *hash_index);
#endif
