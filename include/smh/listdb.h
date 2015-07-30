/**
 * @file listdb.h
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
 * @brief Declaration of structures and functions on list databases
 */
#ifndef LISTDB_H
#define LISTDB_H

#include "array_lists.h"

typedef struct ListDB{
     uint size;
     uint dim;
     List *lists;
}ListDB;

/************************ Function prototypes ************************/
void listdb_init(ListDB *);
ListDB listdb_create(uint, uint);
ListDB listdb_random(uint, uint, uint);
void listdb_clear(ListDB *);
void listdb_destroy(ListDB *);
void listdb_print(ListDB *);
void listdb_print_multi(ListDB *, List *);
void listdb_print_range(ListDB *, uint, uint);
int listdb_size_compare(const void *, const void *);
int listdb_size_compare_back(const void *, const void *);
int listdb_score_compare(const void *, const void *);
int listdb_score_compare_back(const void *, const void *);
void listdb_sort_by_size(ListDB *);
void listdb_sort_by_size_back(ListDB *);
Score *listdb_compute_scores(ListDB *, double (*)(List *));
void listdb_swap_all(ListDB *, Score *);
void listdb_sort_by_score(ListDB *, double (*)(List *));
void listdb_sort_by_score_back(ListDB *, double (*)(List *));
void listdb_apply_to_all(ListDB *, void (*)(List *));
void listdb_apply_to_range(ListDB *, void (*)(List *), uint, uint);
void listdb_apply_to_multi(ListDB *, void (*)(List *), List *);
void listdb_push(ListDB *, List *);
void listdb_pop(ListDB *);
void listdb_pop_multi(ListDB *, uint);
void listdb_pop_until(ListDB *, uint);
void listdb_delete_position(ListDB *, uint);
void listdb_delete_range(ListDB *, uint, uint);
void listdb_delete_smallest(ListDB *, uint);
void listdb_delete_largest(ListDB *, uint);
void listdb_insert(ListDB *, List *, uint);
void listdb_append(ListDB *, ListDB *);
void listdb_append_lists_delete(ListDB *, uint, uint);
void listdb_append_lists_destroy(ListDB *, uint, uint);
ListDB listdb_load_from_file(char *);
void listdb_save_to_file(char *, ListDB *);
#endif
