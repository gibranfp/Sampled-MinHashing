#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "listdb.h"

#define red   "\033[0;31m"        /* 0 -> normal ;  31 -> red */
#define cyan  "\033[0;36m"        /* 1 -> bold ;  36 -> cyan */
#define green "\033[0;32m"        /* 4 -> underline ;  32 -> green */
#define blue  "\033[0;34m"        /* 9 -> strike ;  34 -> blue */
 
#define black  "\033[0;30m"
#define brown  "\033[0;33m"
#define magenta  "\033[0;35m"
#define gray  "\033[0;37m"
 
#define none   "\033[0m"        /* to flush the previous property */

#define MAX_LIST_SIZE 150
#define ELEMENT_MAX_VALUE 15

void test_random_sort_delete_print(void)
{
	ListDB listdb = listdb_random(100, MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	printf("%s=========================\nOriginal Database\n=========================\n", brown);
	listdb_apply_to_all(&listdb, list_sort_by_item);
	listdb_apply_to_all(&listdb, list_unique);
	listdb_sort_by_size_back(&listdb);
	listdb_apply_to_all(&listdb, list_sort_by_frequency_back);
	listdb_print(&listdb);
   
	uint low = (rand() % listdb.size);
	uint high = (rand() % (listdb.size - low)) + low;
	printf("%s=========================\nDatabase (%d-%d)\n=========================\n", magenta, low, high);
	listdb_print_range(&listdb, low, high);

	printf("%s", none);

     
	List positions = list_random(listdb.size, listdb.size);
	list_sort_by_item(&positions);
	list_unique(&positions);
     
	printf("%sRandom positions ::: ", blue);
	list_print(&positions);
	printf("%s=========================\nDatabase (%d positions)\n=========================\n", cyan, positions.size);
	listdb_print_multi(&listdb, &positions);

	printf("%s=========================\nDatabase (larger than 2)\n=========================\n", green);
	listdb_delete_smallest(&listdb, 3);
	listdb_print(&listdb);

	printf("%s=========================\nDatabase (smaller than 5)\n=========================\n", blue);
	listdb_delete_largest(&listdb, 4);
	listdb_print(&listdb);
     
	printf("%s", none);
}

void test_delete_insert_push(void)
{
	ListDB listdb = listdb_random(10, MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	printf("%s=========================\nOriginal Database\n=========================\n", brown);
	listdb_apply_to_all(&listdb, list_sort_by_item);
	listdb_apply_to_all(&listdb, list_unique);
	listdb_sort_by_size_back(&listdb);
	listdb_print(&listdb);

	uint position = (rand() % listdb.size);
	listdb_delete_position(&listdb, position);
	printf("%s=========================\nDatabase without list %d\n=========================\n", blue, position);
	listdb_print(&listdb);
  
	uint low = (rand() % listdb.size);
	uint high = (rand() % (listdb.size - low)) + low;
	printf("%s=========================\nDatabase (%d-%d deleted)\n=========================\n", magenta, low, high);
	listdb_delete_range(&listdb,low, high);
	listdb_print(&listdb);
     
	List list = list_random(MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	listdb_push(&listdb, &list);
	printf("%s=========================\nDatabase with list pushed\n=========================\n", cyan);
	listdb_print(&listdb);
    
	list = list_random(MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	position = (rand() % listdb.size + 1);
	listdb_insert(&listdb, &list, 0);
	printf("%s=========================\nDatabase with list %d inserted\n=========================\n", green, position);
	listdb_print(&listdb);
}

void test_append_add(void)
{
	ListDB listdb = listdb_random(10, MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	printf("%s=========================\nOriginal Database\n=========================\n", brown);
	listdb_apply_to_all(&listdb, list_sort_by_item);
	listdb_apply_to_all(&listdb, list_unique);
	listdb_sort_by_size_back(&listdb);
	listdb_print(&listdb);

	uint position1 = rand() % listdb.size;
	uint position2 = rand() % listdb.size;
	while (position1 == position2)
		position2 = rand() % listdb.size;	  
	listdb_append_lists_destroy(&listdb, position1, position2);
	printf("%s=========================\nDatabase appended (destroy) %d and %d\n=========================\n", red, position1, position2);
	listdb_apply_to_all(&listdb, list_sort_by_item);
	listdb_print(&listdb);

	position1 = rand() % listdb.size;
	position2 = rand() % listdb.size;
	listdb_add_lists_delete(&listdb, position1, position2);
	printf("%s=========================\nDatabase appended (delete) %d and %d\n=========================\n", magenta, position1, position2);
	listdb_apply_to_all(&listdb, list_sort_by_item);
	listdb_print(&listdb);

	position1 = rand() % listdb.size;
	position2 = rand() % listdb.size;
	while (position1 == position2)
		position2 = rand() % listdb.size;	  
	listdb_append_lists_destroy(&listdb, position1, position2);
	printf("%s=========================\nDatabase added (destroy) %d and %d\n=========================\n", cyan, position1, position2);
	listdb_apply_to_all(&listdb, list_sort_by_item);
	listdb_print(&listdb);

	position1 = rand() % listdb.size;
	position2 = rand() % listdb.size;
	listdb_add_lists_delete(&listdb, position1, position2);
	printf("%s=========================\nDatabase added (delete) %d and %d\n=========================\n", green, position1, position2);
	listdb_apply_to_all(&listdb, list_sort_by_item);
	listdb_print(&listdb);
    
	ListDB listdb2 = listdb_random(10, MAX_LIST_SIZE, ELEMENT_MAX_VALUE);
	printf("%s=========================\nOriginal Database 2\n=========================\n", brown);
	listdb_apply_to_all(&listdb2, list_sort_by_item);
	listdb_apply_to_all(&listdb2, list_unique);
	listdb_sort_by_size_back(&listdb2);
	listdb_print(&listdb2);
	
	listdb_append(&listdb, &listdb2);
	
	printf("%s=========================\nDatabases appended\n=========================\n", blue);
	listdb_print(&listdb);
	
	printf("%s", none);
}

void test_load(char *input, char *output)
{
	ListDB listdb = listdb_load_from_file(input);
	listdb_save_to_file(output, &listdb);
	printf ("Read database of %d lists (max item = %d)\n", listdb.size, listdb.dim);
}

int main(int argc, char **argv)
{
	srand((long int) time(NULL));
	/* test_load(argv[1], argv[2]); */
	/* test_delete_insert_push(); */
	test_append_add(); 
	/* test_random_sort_delete_print(); */
     

	return 0;
}
