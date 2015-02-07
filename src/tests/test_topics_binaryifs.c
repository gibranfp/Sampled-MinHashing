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

#define MAX_LIST_SIZE 10
#define ELEMENT_MAX_VALUE 15

uint topics_sort_partition(ListDB *topics, double *coef, uint left, uint right)
{
	double pval = coef[right];
	uint i = left, j;
	for (j = left; j < right; j++){
		if (coef[j] >= pval){
			List tmp = topics->lists[i];
			topics->lists[i] = topics->lists[j];
			topics->lists[j] = tmp;
			double tmpcoef = coef[i];
			coef[i] = coef[j];
			coef[j] = tmpcoef;
			i++;
		}
	}

	List tmp = topics->lists[i];
	topics->lists[i] = topics->lists[right];
	topics->lists[right] = tmp;
	double tmpcoef = coef[i];
	coef[i] = coef[right];
	coef[right] = tmpcoef;
}

void topics_sort_custom(ListDB *topics, double *coef, uint left, uint right)
{
	int pivot;
     
	if (left < right){
		pivot = topics_sort_partition(topics, coef, left, right);
		topics_sort_custom(topics, coef, left, pivot - 1);
		topics_sort_custom(topics, coef, pivot + 1, right);
	}
     
}

double topics_jaccard(List *list1, List *list2, uint *topic_inter)
{
	uint intersection_size = list_intersection_size(list1, list2);
	uint union_size = (list1->size + list2->size) - intersection_size;
     
	*topic_inter = intersection_size;
	if (union_size > 0)
		return (double) intersection_size / (double)union_size;
	else
		return INFINITY;
}


ListDB listdb_read_weighted(char *filename)
{
	int i, j;
	FILE *file;
	ListDB listdb;

	printf("Reading sets from %s . . . \n", filename);
	if (!(file = fopen(filename,"r"))){
		fprintf(stderr,"Error: Could not open file %s\n", filename);
		exit(EXIT_FAILURE);
	}
     
	uint dim, num;
	// reads dimensionality and number of sets
	fscanf(file,"%u %u", &dim, &num); 
	listdb.size = num;
	listdb.dim = dim;
     
	listdb.lists = (List *) malloc(listdb.size * sizeof(List));
	for (i = 0; i < num; i ++){     // reads sets
		uint size;
		fscanf(file,"%u", &size); // cardinality of ith set
		listdb.lists[i].size = size;
		listdb.lists[i].data = (Item *) malloc(listdb.lists[i].size * sizeof(Item));
		for (j = 0; j < listdb.lists[i].size; j++){ // reads set items
			uint id, freq;
			fscanf(file,"%u %u", &id, &freq);
			listdb.lists[i].data[j].item = id;
			listdb.lists[i].data[j].freq = freq;
		}
	}
	if (fclose(file)){
		fprintf(stderr,"Error: Could not close file %s\n", filename);
		exit(EXIT_FAILURE);
	}
         
	printf("%u sets read, Dimensionality = %u\n", listdb.size, listdb.dim);

	return listdb;
}

ListDB listdb_read_binary(char *filename)
{
	int i, j;
	FILE *file;
	ListDB listdb;

	printf("Reading sets from %s . . . \n", filename);
	if (!(file = fopen(filename,"r"))){
		fprintf(stderr,"Error: Could not open file %s\n", filename);
		exit(EXIT_FAILURE);
	}
     
	uint dim, num;
	// reads dimensionality and number of sets
	fscanf(file,"%u %u", &dim, &num); 
	listdb.size = num;
	listdb.dim = dim;
     
	listdb.lists = (List *) malloc(listdb.size * sizeof(List));
	for (i = 0; i < num; i ++){     // reads sets
		uint size;
		fscanf(file,"%u", &size); // cardinality of ith set
		listdb.lists[i].size = size;
		listdb.lists[i].data = (Item *) malloc(listdb.lists[i].size * sizeof(Item));
		for (j = 0; j < listdb.lists[i].size; j++){ // reads set items
			uint id, freq;
			fscanf(file,"%u", &id);
			listdb.lists[i].data[j].item = id;
			listdb.lists[i].data[j].freq = 1;
		}
	}
	if (fclose(file)){
		fprintf(stderr,"Error: Could not close file %s\n", filename);
		exit(EXIT_FAILURE);
	}
         
	printf("%u sets read, Dimensionality = %u\n", listdb.size, listdb.dim);

	return listdb;
}

void rank_topic2(char *ifname, char *topicname)
{
	uint i, j;
	ListDB ifindex = listdb_read_binary(ifname);
	ListDB topics = listdb_read_weighted(topicname);
	double *coef = (double *) calloc(topics.size, sizeof(double));
     
	for (i = 0; i < topics.size; i++)
	{
		if (topics.lists[i].size > 1){
			double topic_jaccard = 0.0;
			double topic_inter = 0.0;
			uint ref = topics.lists[i].data[0].item;
			for (j = 0; j < topics.lists[i].size; j++){
				uint term = topics.lists[i].data[j].item;;
				uint inter;
				topic_jaccard += (double) topics_jaccard(&ifindex.lists[ref], 
														 &ifindex.lists[term], &inter);
     		    topic_inter += (double)inter;
			}
			coef[i] = (double) topic_jaccard * (double) topic_inter * (double) topics.lists[i].size;
		}
		else
			coef[i] = 0.0;	  
	}
	 
	char output[10000];
	strcpy (output, topicname);
	strcat (output,".scores");
	FILE *file;
	printf("Writing coefficients to %s . . . \n", output);
	file = fopen(output,"w");
	for (i = 0; i < topics.size; i ++)
		fprintf(file,"%lf\n", coef[i]);
	fclose(file);
}

void rank_topic(char *ifname, char *topicname)
{
	uint i, j;
	ListDB ifindex = listdb_read_binary(ifname);
	ListDB topics = listdb_read_weighted(topicname);
	double *coef = (double *) calloc(topics.size, sizeof(double));
     
	for (i = 0; i < topics.size; i++)
	{
		if (topics.lists[i].size > 1){
			double topic_jaccard = 0.0;
			double topic_inter = 0.0;
			uint ref = topics.lists[i].data[0].item;
			for (j = 1; j <= 100 && j < topics.lists[i].size; j++){
				uint term = topics.lists[i].data[j].item;;
				uint inter;
				topic_jaccard += (double) topics_jaccard(&ifindex.lists[ref], 
														 &ifindex.lists[term], &inter);
     		    topic_inter += (double)inter;
			}
			coef[i] = (double) topic_jaccard * (double) topic_inter * (double) topics.lists[i].size;
		}
		else
			coef[i] = 0.0;	  
	}

	char output[10000];
	strcpy (output, topicname);
	strcat (output,".scores");
	FILE *file;
	printf("Writing coefficients to %s . . . \n", output);
	file = fopen(output,"w");
	for (i = 0; i < topics.size; i ++)
		fprintf(file,"%lf\n", coef[i]);
	fclose(file);
}

int main(int argc, char **argv)
{
	rank_topic(argv[1], argv[2]);
     
	return 0;
}
