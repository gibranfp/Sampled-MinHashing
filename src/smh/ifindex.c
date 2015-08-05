/**
 * @file ifindex.c
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
 * @brief Utility functions for reading and writing inverted file indexes. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "ifindex.h"

/**
 * @brief Makes a query to the database
 *
 * @param ifindex Inverted file index
 * @param query Query list
 *
 * @return Query result
 */
List ifindex_query(ListDB *ifindex, List *query)
{
	uint i;
	List query_result;
	
	list_init(&query_result);
	for (i = 0; i < query->size; i++) //retrieves each list in inverted
		list_append(&query_result, &ifindex->lists[query->data[i].item]);
	list_sort_by_item(&query_result);
	list_unique(&query_result);

	return query_result;
}

/**
 * @brief Makes multiple queries to a database
 *
 * @param ifindex Inverted file index
 * @param queries Query lists
 *
 * @return Query results
 */
ListDB ifindex_query_multi(ListDB *ifindex, ListDB *queries)
{
	uint i;
	ListDB query_results = listdb_create(queries->size, ifindex->dim);
	for (i = 0; i < queries->size; i++) //retrieves each list in inverted
		query_results.lists[i] = ifindex_query(ifindex, &queries->lists[i]);
     
	return query_results;
}

/**
 * @brief Discards all lists that are less frequent than a given frequency
 *
 * @param query_results Lists
 * @param min_freq Minimum frequency
 */
void ifindex_discard_less_frequent(ListDB *query_results, uint min_freq)
{
	uint i;
	for (i = 0; i < query_results->size; i++)
		list_delete_less_frequent(&query_results->lists[i], min_freq);
}

/**
 * @brief Discards all lists that are more frequent than a given frequency
 *
 * @param query_results Lists
 * @param max_freq Maximum frequency
 */
void ifindex_discard_more_frequent(ListDB *query_results, uint max_freq)
{
	uint i;
	for (i = 0; i < query_results->size; i++)
		list_delete_more_frequent(&query_results->lists[i], max_freq);
}

/**
 * @brief Ranks query results based on document hits
 *
 * @param query_results Database of lists with query results
 */
void ifindex_rank_more_frequent(ListDB *query_results)
{
	listdb_apply_to_all(query_results, list_sort_by_frequency_back);
}
     
/**
 * @brief Makes inverted file from a given corpus
 *
 * @param corpus Corpus
 *
 * @return Inverted file index
 */
ListDB ifindex_make_from_corpus(ListDB *corpus)
{
	uint i, j;
	uint tid;
	
	// reads corpus and creates inverted file
	ListDB ifindex = listdb_create(corpus->dim, corpus->size);
	for (i = 0; i < corpus->size; i++) {
		for (j = 0; j < corpus->lists[i].size; j++) {
			tid = corpus->lists[i].data[j].item;
			Item item = {i, corpus->lists[i].data[j].freq};
			list_push(&ifindex.lists[tid], item);
		}
	}

	return ifindex;
}

/**
 * @brief Computes weights of an inverted file structure
 *
 * @param ifindex Inverted file index
 * @param corpus Corpus
 * @param wg Function to compute weight
 */
void ifindex_weight(ListDB *ifindex, ListDB *corpus, double (*wg)(uint, uint, uint))
{
	uint i, j;
	double wval;

	// performs term weighting
	for (i = 0; i < ifindex->size; i++) {
		for (j = 0; j < ifindex->lists[i].size; j++) {
			wval = (double) wg(ifindex->lists[i].data[j].freq, ifindex->lists[i].size, ifindex->dim);
			ifindex->lists[i].data[j].freq = intweight(wval);
			if (ifindex->lists[i].data[j].freq == 0)
				ifindex->lists[i].data[j].freq = 1;
		}
	}
}
