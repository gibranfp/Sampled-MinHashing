#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "textio.h"

/**
 * @brief Reads vocabulary in NIPS format:
 *             term1 = id1 = termfreq1 termdocfreq1
 *                        ...
 *
 * @param filename File with the vocabulary
 * @param vocab Vocabulary
 * @param termod Term ids
 * @param corpfreq Number of times the term appears in the corpus
 * @param docfreq Number of documents where the term appears
 * @param vocsize Number of terms in the vocabulary
 */
void text_load_vocabulary(char *filename, char ***vocab, uint **termid, uint **corpfreq, 
						  uint **docfreq, uint *vocsize)
{
	FILE *fp;
	fp = fopen(filename, "r");
	if (fp == NULL){
		fprintf(stderr,"Could not open file %s\n", filename);
		exit(EXIT_FAILURE);
	}
     
	char *line = NULL;
	char *token;
	size_t len = 0;
	ssize_t read;
	uint *cf = NULL;
	uint *df = NULL;
	uint *id = NULL;
	uint vsize = 0;
	char **v = NULL;

	while ((read = getline(&line, &len, fp)) != -1){
		// term
		token = strtok (line, " = ");
		v = (char **) realloc(v, (vsize + 1) * sizeof(char *));
		v[vsize] = (char *) malloc((strlen(token) + 1) * sizeof(char));
		strcpy(v[vsize], token);
		// term id
		token = strtok (NULL, " = ");
		id = (uint *) realloc(id, (vsize + 1) * sizeof(uint));
		id[vsize] = atoi(token);
		// term frequency
		token = strtok (NULL, " = ");
		cf = (uint *) realloc(cf, (vsize + 1) * sizeof(uint));
		cf[vsize] = atoi(token);
		// term document frequency
		token = strtok (NULL," ");
		df = (uint *) realloc(df, (vsize + 1) * sizeof(uint));
		df[vsize] = atoi(token);
		vsize++;
	}
	free(line);
	fclose(fp);

	*vocab = v;
	*termid = id;
	*corpfreq = cf;
	*docfreq = df;
	*vocsize = vsize;
}

/**
 * @brief Prints a list of ids as a word list
 *
 * @param id_list List of ids
 * @param vocabulary Vocabulary
 */
void text_print_idlist_as_words(List *id_list, char **vocabulary)
{
	uint i;
	for (i = 0; i < id_list->size; i++)
		printf ("%s ", vocabulary[id_list->data[i].item]);
}

/**
 * @brief Prints a database of id lists as word lists
 *
 * @param id_list List of ids
 * @param vocabulary Vocabulary
 */
void text_print_idlistdb_as_words(ListDB *id_lists, char **vocabulary)
{
	uint i;
	for (i = 0; i < id_lists->size; i++){
		text_print_idlist_as_words(&id_lists->lists[i], vocabulary);
		printf ("\n");
	}    
}

/**
 * @brief Saves a database of id lists as word lists
 *
 * @param filename File where the word lists will be saved
 * @param id_lists ID lists
 * @param vocabulary Vocabulary
 */
void text_save_idlistdb_as_words(char *filename, ListDB *id_lists, char **vocabulary)
{
	FILE *file;     
	if (!(file = fopen(filename,"w"))){
		fprintf(stderr,"Error: Could not create file %s\n", filename);
		exit(EXIT_FAILURE);
	}
     
	uint i, j;
	for (i = 0; i < id_lists->size; i++){
		for (j = 0; j < id_lists->lists[i].size; j++)
			fprintf (file, "%s[%d] ", vocabulary[id_lists->lists[i].data[j].item], 
					 id_lists->lists[i].data[j].freq);
		fprintf (file, "\n");
	}

	if (fclose(file)){
		fprintf(stderr,"Error: Could not close file %s\n", filename);
		exit(EXIT_FAILURE);
	}
}
