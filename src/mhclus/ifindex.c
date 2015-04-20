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
	ListDB query_results = listdb_create(queries->size);
	for (i = 0; i < queries->size; i++) //retrieves each list in inverted
		query_results.lists[i] = ifindex_query(ifindex, &queries->lists[i]);
     
	return query_results;
}

/**
 * @brief Discards all lists that are less frequent than a given frequency
 *
 * @param query_results Lists
 * @param query Query list
 *
 * @return Result
 */
void ifindex_discard_less_frequent(ListDB *query_results, uint min_freq)
{
	uint i;
	for (i = 0; i < query_results->size; i++)
		list_delete_less_frequent(&query_results->lists[i], min_freq);
}

/**
 * @brief Returns a list that include the items in the lists given by the query
 *
 * @param ifindex Inverted file index
 * @param query Query list
 *
 * @return Result
 */
void ifindex_discard_more_frequent(ListDB *query_results, uint max_freq)
{
	uint i;
	for (i = 0; i < query_results->size; i++)
		list_delete_more_frequent(&query_results->lists[i], max_freq);
}
     
/**
 * @brief Returns a list that include the items in the lists given by the query
 *
 * @param ifindex Inverted file index
 * @param query Query list
 *
 * @return Result
 */
void ifindex_sort_custom(ListDB *ifindex, ListDB *queries, ListDB *query_results, 
						 int (*custom_compare)(const void *, const void *))
{
	uint i;
	for (i = 0; i < query_results->size; i++)
		qsort(&query_results[i], query_results[i].size, sizeof(Item), 
			   custom_compare);
}

/* /\** */
/*  * @brief Makes inverted file from a given corpus */
/*  * */
/*  * @param corpus Corpus */
/*  * @param termfreq Frequency of each term in the document */
/*  * @param doc_card Number of terms in each document */
/*  * @param corpus_size Number of documents in the corpus */
/*  *\/ */
/* ListDB make_ifs(uint **corpus, uint **termfreq, uint *doccard, uint corpsize,  */
/* 				uint *corpfreq, uint *docfreq, uint vocsize, */
/* 				double (*wg)(const void*,const void*,const void*)) */
/* { */
/* 	uint i, j; */
/* 	uint tid; */
/* 	double wval; */
	
/* 	ListDB ifindex = listdb_create(vocsize); */

/* 	for (i = 0; i < vocsize; i++) { */
/* 		ifindex.lists[i] = (uint *) malloc(docfreq[i] * sizeof(uint)); */
/* 		tw[i] = (uint *) malloc(docfreq[i] * sizeof(uint)); */
/* 	} */

/* 	for (i = 0; i < corpsize; i++) { */
/* 		for (j = 0; j < doccard[i]; j++) { */
/* 			tid = corpus[i][j]; */
/* 			db[tid][tc[tid]] = i; */
/* 			wval = (double) termfreq[i][j] * log((double) corpsize / (double) docfreq[tid]); */
/* 			tw[tid][tc[tid]] = intweight(wval); */
/* 			if (tw[tid][tc[tid]] == 0) { */
/* 				tw[tid][tc[tid]] = 1; */
/* 			} */
/* 			tc[tid]++; */
/* 		} */
/* 	} */
     
/* 	*ifs = db; */
/* 	*weight = tw; */
/* 	*ifs_card = tc; */
/* } */
