/**
 * @file array_lists.h
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
 * @brief Declaration of structures and functions on array lists
 */
#ifndef ARRAY_LISTS_H
#define ARRAY_LISTS_H

#include "types.h"

typedef struct Item {
     uint item;
     uint freq;
}Item;


typedef struct List{
     uint size;
     Item *data;
}List;

typedef struct Score{
	double value;
	uint index;
}Score;

/************************ Function prototypes ************************/
void list_init(List *);
List list_create(uint);
List list_random(uint, uint);
void list_destroy(List *);
Item list_make_item(uint, uint);
Item *list_find(List *, Item);
Item *list_min_item(List *);
Item *list_max_item(List *);
Item *list_min_freq(List *);
Item *list_max_freq(List *);
uint list_sum_freq(List *);
int list_item_compare(const void *, const void *);
int list_item_compare_back(const void *, const void *);
int list_frequency_compare(const void *, const void *);
int list_frequency_compare_back(const void *, const void *);
int list_score_compare(const void *a, const void *b);
int list_score_compare_back(const void *a, const void *b);
Item *list_binary_search(List *, Item);
void list_sort_by_item(List *);
void list_sort_by_item_back(List *);
void list_sort_by_frequency(List *);
void list_sort_by_frequency_back(List *);
void list_print(List *);
void list_print_multi(List *, List *);
void list_print_range(List *, uint, uint);
void list_push(List *, Item);
void list_push_range(List *, List *, uint, uint);
void list_pop(List *);
void list_pop_multi(List *, uint);
void list_pop_until(List *, uint);
void list_delete_position(List *, uint);
void list_delete_item(List *, Item);
void list_delete_range(List *, uint, uint);
void list_delete_less_frequent(List *, uint);
void list_delete_more_frequent(List *, uint);
void list_unique(List *);
void list_insert(List *, Item, uint);
List list_duplicate(List *);
List list_copy_range(List *, uint, uint);
List list_concat(List *, List *);
void list_append(List *, List *);
void list_add(List *, List *);
List list_union(List *, List *);
uint list_union_size(List *, List *);
List list_intersection(List *, List *);
uint list_intersection_size(List *, List *);
List list_difference(List *, List *);
uint list_difference_size(List *, List *);
double list_jaccard(List *, List *);
double list_overlap(List *, List *);
double list_weighted_similarity(List *, List *, double *);
double list_histogram_intersection(List *, List *);
double list_weighted_histogram_intersection(List *, List *, double *);
uint list_equal(List *, List *);
#endif
