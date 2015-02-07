/**
 * @file ifindex.h
 * @author Gibran Fuentes Pineda <gibranfp@turing.iimas.unam.mx>
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
 * @brief Declaration of structures for handling lists and inverted file structures.
 */
#ifndef IFINDEX_H
#define IFINDEX_H

#include "listdb.h"

/************************ Function prototypes ************************/
List ifindex_query(ListDB *, List *);
ListDB ifindex_query_multi(ListDB *, ListDB *);
void ifindex_discard_less_frequent(ListDB *, uint);
void ifindex_rank_more_frequent(ListDB *);
void ifindex_sort_custom(ListDB *, ListDB *, ListDB *,  
						 int (*)(const void *, const void *));
#endif
