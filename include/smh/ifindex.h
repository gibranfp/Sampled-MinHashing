/**
 * @file ifindex.h
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
 * General Public License for mor e details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @brief Declaration of structures for handling lists and inverted file structures.
 */
#ifndef IFINDEX_H
#define IFINDEX_H

#include "listdb.h"
#include "weights.h"

/************************ Function prototypes ************************/
List ifindex_query(ListDB *, List *);
ListDB ifindex_query_multi(ListDB *, ListDB *);
void ifindex_discard_less_frequent(ListDB *, uint);
void ifindex_discard_more_frequent(ListDB *, uint);
void ifindex_rank_more_frequent(ListDB *);
ListDB ifindex_make_from_corpus(ListDB *);
void ifindex_weight(ListDB *, ListDB *, double (*)(uint, uint, uint, uint, uint, uint));
#endif
