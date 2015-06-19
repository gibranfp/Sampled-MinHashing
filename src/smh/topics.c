/**
 * @file topics.c
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
 * @brief Definition of structures for handling lists and inverted file structures.
 */
#include <stdio.h>
#include <stdlib.h>
#include "topics.h"

/**
 * @brief Retrives documents containing a specific topic from a database
 *
 * @param ifindex Inverted file index
 * @param topic Topic to be searched
 */
List topics_retrieve_docs(ListDB *ifindex, List *topic)
{
	return ifindex_query(ifindex, topic);
}

/**
 * @brief Retrieves documents for each given topic from a database
 *
 * @param ifindex Inverted file index
 * @param topics Database of topics to be searched
 */
ListDB topics_retrieve_docs_multi(ListDB *ifindex, ListDB *topics)
{
	return ifindex_query_multi(ifindex, topics);
}

/**
 * @brief Ranks a database of documents by relevance
 *
 * @param ifindex Inverted file index
 * @param docs Database of documents given as lists
 * @param topics Database of topics to be searched
 */
int topics_compare_relevance(ListDB *ifindex, ListDB *docs, List *topic_a, List *retrieved_a, 
							 List *topic_b, List *retrieved_b)
{
	int i;
	double prob_a = 0.0;
	double prob_b = 0.0;

	for (i = 0; i < retrieved_a->size; ++i)
		prob_a += -log(retrieved_a->data[i].freq / docs->lists[retrieved_a->data[i].item].size);
     
	for (i = 0; i < retrieved_b->size; ++i){
		double doc_ratio = retrieved_b->data[i].freq / docs->lists[retrieved_b->data[i].item].size;
		double topic_ratio = retrieved_b->data[i].freq / topic_a->size;
		prob_b += -log();
	}

	if (prob_a >= prob_b)
		return 1;
	else
		return 0;
}

/**
 * @brief Ranks a database of documents by relevance
 *
 * @param ifindex Inverted file index
 * @param docs Database of documents given as lists
 * @param topics Database of topics to be searched
 */
void topics_rank_by_relevance(ListDB *ifindex, ListDB *docs, ListDB *topics)
{
	ListDB retrieved = topics_retrieve_all_docs(ifindex, topics);
	listdb_apply_to_all(&retrieved, list_sort_by_frequency_back);
     
	topics_sort_custom(ifindex, docs, topics, &retrieved, 0, topics->size,
					   topics_compare_relevance);
}

/**
 * @brief Ranks a database of documents by relevance
 *
 * @param ifindex Inverted file index
 * @param docs Database of documents given as lists
 * @param topics Database of topics to be searched
 */
void topics_rank_by_relevance_back(ListDB *ifindex, ListDB *docs, ListDB *topics)
{
	ListDB retrieved = topics_retrieve_all_docs(ifindex, topics);
     
	topics_sort_custom(ifindex, docs, topics, &retrieved, 0, topics->size,
					   topics_compare_relevance_back);
}

/**
 * @brief Ranks a database of documents by relevance
 *
 * @param ifindex Inverted file index
 * @param docs Database of documents given as lists
 * @param topics Database of topics to be searched
 */
void topics_rank_custom(ListDB *, ListDB *, uint low, uint high,
						int (*topics_compare)(const void*,const void*,const void*,
											  const void*,const void*,const void*))
{
	ListDB retrieved = topics_retrieve_all_docs(ifindex, topics);
	  
	topics_sort_custom(ifindex, docs, topics, &retrieved, 0, topics->size,
					   topics_compare);
}

/**
 * @brief Swaps two lists
 *
 * @param first First list
 * @param second Second list
 */
void topics_sort_swap(List *first, List *second)
{
	List tmp = *first;
	*first = *second;
	*second = tmp;
}

/**
 * @brief Sort a database of lists
 *
 * @param ifindex Inverted file index
 * @param docs Database of documents given as lists
 * @param topics Database of topics to be searched
 */
void topics_sort_custom(ListDB *ifindex, ListDB *docs, ListDB *topics,
						ListDB *retrieved, uint left, uint right,
						int (*topics_compare)(const void*,const void*,const void*,
											  const void*,const void*,const void*))
{
	int pivot;
	if (left < right){
		pivot = topics_sort_partition(ifindex, docs, topics, retrieved,
									  left, right, topics_compare);
		topics_sort_custom(ifindex, docs, topics, retrieved, left, pivot - 1,
						   topics_compare);
		topics_sort_custom(ifindex, docs, topics, retrieved, pivot + 1, right,
						   topics_compare);
	}
     
}

uint topics_sort_partition(ListDB *ifindex, ListDB *docs, ListDB *topics, ListDB *retrieved,
						   uint left, uint right, uint pivotIndex,
						   int (*topics_compare)(const void*,const void*, const void*,
												 const void*,const void*,const void*))
{
	uint i = left, j;
	for (j = left; j < right; j++){
		if (topics_compare(ifindex, docs, &topics->lists[j], &retrieved->lists[j],
						   &topics->lists[right], &retrieved->lists[right])){
			topics_sort_swap(&topics->lists[i], &topics->lists[j]);
			topics_sort_swap(&retrieved->lists[i], &retrieved->lists[j]);
			i++;
		}
	}

	topics_sort_swap(&topics->lists[i], &topics->lists[right]);
	topics_sort_swap(&retrieved->lists[i], &retrieved->lists[right]);
}
