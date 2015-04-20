#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "ifindex.h"
#include "minhash.h"

#define red "\033[0;31m"
#define cyan "\033[0;36m"
#define green "\033[0;32m"
#define blue "\033[0;34m"
#define brown "\033[0;33m"
#define magenta "\033[0;35m"
#define none "\033[0m"

#define MAX_LIST_SIZE 10
#define ELEMENT_MAX_VALUE 15

void test_init_create_print(void)
{
	HashTable htable1;
	mh_init(&htable1);
	mh_print_head(&htable1);
	mh_print_table(&htable1);

	
	HashTable htable2 = mh_create(256, 3, 100);
	mh_print_head(&htable2);

	mh_generate_permutations(htable2.dim, htable2.tuple_size, htable2.permutations);

	uint i, j;
	printf("================= Permutations ==================\n");
	printf("Integers\n");
	for (i = 0; i < htable2.tuple_size; i++){
		printf("tuple %d: {",i);
		for (j = 0; j < htable2.dim; j++)
			printf("[%d]%d ", j,
				   htable2.permutations[i * htable2.dim + j].random_int);
		printf("}\n");
	}

	printf("Double\n");
	for (i = 0; i < htable2.tuple_size; i++){
		printf("tuple %d: {",i);
		for (j = 0; j < htable2.dim; j++){
			printf("[%d]%lf ", j,
				   htable2.permutations[i * htable2.dim + j].random_double);
		}
		printf("}\n");
	}

	mh_destroy(&htable2);
	mh_print_head(&htable2);
	printf("%s", none);

}

void test_hash(void)
{
	HashTable htable = mh_create(256, 3, 12);
	mh_print_head(&htable);

	mh_generate_permutations(htable.dim, htable.tuple_size, htable.permutations);

	uint i, j;
	printf("================= Permutations ==================\n");
	printf("Integers\n");
	for (i = 0; i < htable.tuple_size; i++){
		printf("tuple %d: {",i);
		for (j = 0; j < htable.dim; j++)
			printf("[%d]%d ", j,
				   htable.permutations[i * htable.dim + j].random_int);
		printf("}\n");
	}

	printf("Double\n");
	for (i = 0; i < htable.tuple_size; i++){
		printf("tuple %d: {",i);
		for (j = 0; j < htable.dim; j++){
			printf("[%d]%lf ", j,
				   htable.permutations[i * htable.dim + j].random_double);
		}
		printf("}\n");
	}

	printf("\nList = ");
	List list = list_random(5,12);
	while (list.size == 0)
		list = list_random(5,12);

	list_unique(&list);
	list_print(&list);
	
	for (i = 0; i < htable.tuple_size; i++) {
		printf("\t");
		for (j = 0; j < list.size; j++) 
			printf("Perm[%u][%u]%u ", i * htable.dim, list.data[j].item, htable.permutations[list.data[j].item].random_int);
		printf("\n\t");

		for (j = 0; j < list.size; j++) 
			printf("Perm[%u][%u]%lf ", i * htable.dim, list.data[j].item, htable.permutations[list.data[j].item].random_double);

		uint index = mh_compute_minhash(&list, &htable.permutations[i * htable.dim]);
		printf("\n\tindex[%u] = %u\n", i, index);
	}

	uint hash_value, bucket1, bucket2;
	mh_univhash(&list, &htable, &hash_value, &bucket1);
	bucket2 = mh_get_index(&list, &htable);
	printf("\nhash_value = %u Candidate bucket = %u Final bucket = %u Table hash_value = %u\n", hash_value, bucket1, bucket2, htable.buckets[bucket2].hash_value);
}

void test_store(void)
{
	List list = list_random(5,12);
	while (list.size == 0)
		list = list_random(5,12);

	list_unique(&list);
	printf("List = ");
	list_print(&list);

	HashTable htable = mh_create(256, 3, 12);
	mh_generate_permutations(htable.dim, htable.tuple_size, htable.permutations);
	uint index = mh_store_list(&list, 0, &htable);
	uint i;
	printf("Index = %u\n", index);
	for (i = 0; i < htable.table_size; i++) {
		printf("[ %u ] ", i);
		list_print(&htable.buckets[i].items);
	}

	printf("Removing list ");
	list_print(&list);
	mh_erase_from_list(&list, &htable);
	for (i = 0; i < htable.table_size; i++) {
		printf("[ %u ] ", i);
		list_print(&htable.buckets[i].items);
	}

	ListDB listdb = listdb_random(12, 12, 12);
	listdb_apply_to_all(&listdb, list_sort_by_item);
	listdb_apply_to_all(&listdb, list_unique);
	listdb_print(&listdb);

	uint *indices;
	indices = malloc(listdb.size * sizeof(uint));
	mh_store_listdb(&listdb, &htable, indices);
	for (i = 0; i < htable.used_buckets.size; i++) {
		printf("[ %u ] ", htable.used_buckets.data[i].item);
		list_print(&htable.buckets[htable.used_buckets.data[i].item].items);
	}
	
	mh_print_head(&htable);
	mh_erase_from_list(&listdb.lists[2], &htable);
	for (i = 0; i < htable.used_buckets.size; i++) {
		printf("[ %u ] ", htable.used_buckets.data[i].item);
		list_print(&htable.buckets[htable.used_buckets.data[i].item].items);
	}
	mh_print_head(&htable);
}

int main()
{
     srand((long int) time(NULL));
     
     /* test_init_create_print(); */
	 /* test_hash(); */
	 test_store();
 
     return 0;
}
