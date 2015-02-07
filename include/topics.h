/**
 * @file topics.h
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
 * @brief Declaration of structures for handling lists and inverted file structures.
 */
#ifndef TOPICS_H
#define TOPICS_H
#include "ifindex.h"

ListDB topics_load_docs(char *);
void topics_load_vocabulary(char *, char ***, uint **, uint **, uint **, uint *);
void topics_print_as_words(ListDB *, char **);
void topics_save_listdb_as_words(char *, ListDB *, char **);
List topics_retrieve_docs(ListDB *, List *);
ListDB topics_load_docs(char *);
void topics_load_vocabulary(char *, char ***, uint **, uint **, uint **, uint *);
void topics_print_as_words(ListDB *, char **);
void topics_save_listdb_as_words(char *, ListDB *, char **);
List topics_retrieve_docs(ListDB *, List *);
int topics_compare_relevance(const void *ifindex, const void *docs, const void *topic_a,
							 const void *retrieved_a, const void *topic_b, 
							 const void *retrieved_b);
void topics_rank_by_relevance_back(ListDB *, ListDB *, ListDB *);
void topics_rank_custom(ListDB *, ListDB *, uint, uint, 
			double (*)(const void*,const void*,const void*, const void*,
				   const void*,const void*));
void topics_sort_custom(ListDB *, ListDB *, ListDB *, ListDB *, uint, uint, 
			double (*)(const void*,const void*,const void*, const void*,
				   const void*,const void*));
uint topics_sort_partition(ListDB *ifindex, ListDB *docs, ListDB *topics, ListDB *retrieved, 
			   uint left, uint right, uint, 
			   double (*)(const void*,const void*, const void*, const void*,
				      const void*,const void*));
#endif
