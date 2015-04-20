#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "array_lists.h"

#define red "\033[0;31m"
#define cyan "\033[0;36m"
#define green "\033[0;32m"
#define blue "\033[0;34m"
#define brown "\033[0;33m"
#define magenta "\033[0;35m"
#define black  "\033[0;30m"
#define none "\033[0m"

#define MAX_LIST_SIZE 50
#define ELEMENT_MAX_VALUE 100

void test_random_push_print(void)
{
	List list = list_random(MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	list_sort_by_item(&list);
	list_unique(&list);
	printf("Original List ::: ");
	list_print(&list);

	List push = list_random(MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	list_sort_by_item(&push);
	list_unique(&push);
	printf("%sList to be pushed ::: ", brown);
	list_print(&push);

	uint low = rand() % push.size;
	uint high = (rand() % (push.size - low)) + low;
	list_push_range(&list, &push, low, high);
	printf("%sList with %d-%d Items From List 2 Pushed ::: ", cyan, low, high);
	list_print(&list);
	printf("%s", none);

	list_sort_by_item(&list);
	list_unique(&list);
	printf("%sList Sorted By Item ::: ", magenta);
	list_print(&list);
     
	low = rand() % list.size;
	high = (rand() % (list.size - low)) + low;
	printf("%sRandom List range %d - %d ::: ", green, low, high);
	list_print_range(&list, low, high);
	printf("%s", none);
     
	List positions = list_random(list.size, list.size);
	list_sort_by_item(&positions);
	list_unique(&positions);
     
	printf("%sRandom positions ::: ", blue);
	list_print(&positions);
	printf("%sPrinting random positions ::: ", black);
	list_print_multi(&list, &positions);
	printf("%s", none);
}

void test_insert_duplicate_copy(void)
{   
	List list = list_random(MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	printf("Random List ::: ");
	list_print(&list);

	List duplicate = list_duplicate(&list);
	printf("%sDuplicate List ::: ", cyan);
	list_print(&duplicate);

	uint low = rand() % list.size;
	uint high = (rand() % (list.size - low)) + low;
	List copy = list_copy_range(&list, low, high);
	printf("%sCopy List Range %d-%d::: ", red, low, high);
	list_print(&copy);
     
	Item random_item;
	random_item.item = rand() % ELEMENT_MAX_VALUE;
	random_item.freq = 1;
	uint random_position = rand() % list.size;
	list_insert(&list, random_item, random_position);
	printf("%sRandom List with %d inserted in %d ::: ", green, random_item.item, random_position);
	list_print(&list);
	printf("%s",none);
}

void test_sort_unique_find_search(void)
{
	uint random_size;
    
	random_size = rand() % MAX_LIST_SIZE;
	List zeros = list_create(random_size);
	printf("Zero List ::: ");
	list_print(&zeros);

	List list = list_random(MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	printf("%sRandom List ::: ", cyan);
	list_print(&list);

	list_sort_by_item(&list);
	printf("%sList Resorted By Item ::: ", red);
	list_print(&list);
     
	printf("%sUnique List ::: ", magenta);
	list_unique(&list);
	list_print(&list);

	list_sort_by_frequency_back(&list);
	printf("%sList Sorted Backward By Frequency ::: ",green);
	list_print(&list);

	list_sort_by_frequency(&list);
	printf("%sList Resorted By Frequency ::: ", blue);
	list_print(&list);
     
	Item query = list.data[rand() % list.size];
	Item *found = list_find(&list, query);
	if (found != NULL)
		printf("%sFind function: %d found in position %d\n", brown, query.item, (int)(found - list.data));    
	else
		printf("%sFind function: %d not found", brown, query.item);
 
	list_sort_by_item_back(&list);
	printf("%sList Sorted Backwards By Item ::: ", black);
	list_print(&list);

	list_sort_by_item(&list);
	printf("%sList Resorted By Item ::: ", cyan);
	list_print(&list);

	query = list.data[rand() % list.size];
	found = list_binary_search(&list, query);
	printf("%sBinary search: %d found in position %d\n", none, query.item, (int)(found - list.data));    
}

void test_min_max(void)
{
	uint random_size;
    
	List list = list_random(MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	printf("%sRandom List ::: ", cyan);
	list_print(&list);

	Item *min_item = list_min_item(&list);
	Item *max_item = list_max_item(&list);
	printf("%sMin item = %d[%d] max item = %d [%d]\n", none, min_item->item, (int)(min_item - list.data), max_item->item, (int)(max_item - list.data));    

	list_sort_by_item(&list);
	list_unique(&list);
	printf("Sorted List ::: ");
	list_print(&list);
}

void test_pop_delete(void)
{
	List list = list_random(MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	list_sort_by_item(&list);
	list_unique(&list);
	printf("Original List ::: ");
	list_print(&list);

	List duplicate = list_duplicate(&list);
	list_pop(&duplicate);
	printf("%sList 1 without last item ::: ", cyan);
	list_print(&duplicate);
   
	list_destroy(&duplicate);
	duplicate = list_duplicate(&list);
	uint random_value = rand() % duplicate.size;
	list_pop_multi(&duplicate, random_value);
	printf("%sList without last %d items ::: ", brown, random_value);
	list_print(&duplicate);
	      
	list_destroy(&duplicate);
	duplicate = list_duplicate(&list);
	uint random_end = rand() % duplicate.size;
	list_pop_until(&duplicate, random_end);
	printf("%sList until %d ::: ", magenta, random_end);
	list_print(&duplicate);

	list_destroy(&duplicate);
	duplicate = list_duplicate(&list);
	uint random_position = rand() % (duplicate.size);
	list_delete_position(&duplicate, random_position);
	printf("%sList wihout %d-d item ::: ", green, random_position);
	list_print(&duplicate);
	printf("%s", none);

	list_destroy(&duplicate);
	duplicate = list_duplicate(&list);
	Item random_item;
	random_position = rand() % (duplicate.size);
	random_item = duplicate.data[random_position];
	list_delete_item(&duplicate, random_item);
	printf("%sList without %d ::: ", red, random_item.item);
	list_print(&duplicate);
	printf("%s", none);
     
	list_destroy(&duplicate);
	duplicate = list_duplicate(&list);
	uint low = rand() % duplicate.size;
	uint high = (rand() % (duplicate.size - low)) + low;
	list_delete_range(&duplicate, low, high);
	printf("%sList without range %d - %d ::: ", blue, low, high);
	list_print(&duplicate);
	printf("%s", none);
}

void test_concat_append_add_union_intersection_difference(void)
{     
	List list1 = list_random(MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	list_sort_by_item(&list1);
	list_unique(&list1);
	printf("Original List 1 ::: ");
	list_print(&list1);

	List list2 = list_random(MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	list_sort_by_item(&list2);
	list_unique(&list2);
	printf("%sOriginal List 2 ::: ", brown);
	list_print(&list2);

	List concat = list_concat(&list1, &list2);
	printf("%sConcatenation List ::: ", cyan);
	list_print(&concat);

	List append = list_duplicate(&list1);
	list_append(&append, &list2);
	printf("%sList 1 appended to list 2 ::: ", magenta);
	list_print(&append);
     
	List add = list_duplicate(&list1);
	list_add(&add, &list2);
	printf("%sList 1 added to list 2 ::: ", green);
	list_print(&add);

	List union_list = list_union(&list1, &list2);	
	printf("%sList Union ::: ", blue);
	list_print(&union_list);

	List intersection_list = list_intersection(&list1, &list2);
	printf("%sList Intersection ::: ", red);
	list_print(&intersection_list);

	List difference_list = list_difference(&list1, &list2);
	printf("%sDifference List ::: ", cyan);
	list_print(&difference_list);

	uint union_size = list_union_size(&list1, &list2);
	printf("%sList Union Size = %d\n", brown, union_size);

	uint intersection_size = list_intersection_size(&list1, &list2);
	printf("%sList Intersection Size = %d\n", black, intersection_size);

	uint difference_size = list_difference_size(&list1, &list2);
	printf("%sList Difference Size = %d\n", black, difference_size);
     
	printf("%s",none);

}

void test_jaccard_overlap_histogramsim(void)
{
	List list1 = list_random(MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	list_sort_by_item(&list1);
	list_unique(&list1);
	printf("Original List 1 ::: ");
	list_print(&list1);

	List list2 = list_random(MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	list_sort_by_item(&list2);
	list_unique(&list2);
	printf("%sOriginal List 2 ::: ", brown);
	list_print(&list2);

	uint *weights = (uint *) malloc(ELEMENT_MAX_VALUE * sizeof(uint));
	uint i;
	printf("%sWeights = ",black);
	for (i = 0; i < ELEMENT_MAX_VALUE; i++){
		weights[i] = rand() % 10;
		printf(" [%d = %d]", i, weights[i]);
	}
	printf("\n");

	double jaccard = list_jaccard(&list1, &list2);
	double overlap = list_overlap(&list1, &list2);
	double hist_inter = list_histogram_intersection(&list1, &list2);
	double weight_hist_inter = (double ) list_weighted_histogram_intersection(&list1, &list2, weights);
	double weight_sim = list_weighted_similarity(&list1, &list2, weights);
	uint inter_size = list_intersection_size(&list1, &list2);
	printf("%sJaccard = %f\n"
		   "%sOverlap = %f\n"
		   "%sHistogram Intersection = %f\n"
		   "%sWeighted Histogram Intersection = %f\n"
		   "%sWeighted Similarity = %f\n"
		   "%sInter size = %u%s\n",
		   cyan, jaccard, magenta, overlap, green, hist_inter, blue, weight_hist_inter, red, weight_sim, cyan, inter_size, none);
}

void test_less_more_frequent(void)
{
	List list = list_random(MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	list_sort_by_item(&list);
	list_unique(&list);
	list_sort_by_frequency_back(&list);
	printf("Original List ::: ");
	list_print(&list);

	List duplicate = list_duplicate(&list);
	list_delete_less_frequent(&duplicate, 5);
	printf("%sList without less frequent items ::: ", cyan);
	list_print(&duplicate);
   
	list_destroy(&duplicate);
	duplicate = list_duplicate(&list);
	list_delete_more_frequent(&duplicate, 8);	
	list_sort_by_frequency_back(&duplicate);
	printf("%sList without more frequent items ::: ", magenta);
	list_print(&duplicate);
	printf ("%s",none);
}

int main()
{
	srand((long int) time(NULL));

	/* test_min_max(); */
	/* test_sort_unique_find_search(); */
	/* test_less_more_frequent(); */
	/* test_jaccard_overlap_histogramsim(); */
	/* test_concat_append_add_union_intersection_difference(); */
	/* test_pop_delete(); */
	/* test_sort_unique_find_search(); */
	/* test_random_push_print(); */
	test_insert_duplicate_copy();
     
     
	return 0;
}
