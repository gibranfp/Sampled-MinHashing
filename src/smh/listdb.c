/**
 * @file listdb.c
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
 * @brief Basic operations on databases of lists.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <float.h>
#include "listdb.h"

/**
 * @brief Initializes a list database structure to zero
 *
 * @param *listdb List database to initialize
 */
void listdb_init(ListDB *listdb)
{     
     listdb->size = 0;
     listdb->dim = 0;
     listdb->lists = NULL;
}

/**
 * @brief Creates a list database structure of a given size with lists initialized to zero
 *
 * @param size Size of the database to create
 *
 * @return Created database
 */
ListDB listdb_create(uint size, uint dim)
{     
     ListDB listdb;
     listdb.size = size;
     listdb.dim = dim;
     listdb.lists = (List *) calloc(size, sizeof(List));

     return listdb;
}

/**
 * @brief Creates a random list database structure
 *
 * @param dbsize Maximum size of the database
 * @param max_size Maximum size of the lists
 * @param max_item Maximum value of items
 *
 * @return Random list
 */
ListDB listdb_random(uint dbsize, uint max_size, uint max_item)
{  
     ListDB listdb;
     uint i;

     listdb = listdb_create(dbsize, max_item);
     for (i = 0; i < dbsize; i++)
          listdb.lists[i] = list_random(max_size, max_item);

     return listdb;
}

/**
 * @brief Clears a list database structure
 *
 * @param *listdb List database to be cleared
 */
void listdb_clear(ListDB *listdb)
{     
     free(listdb->lists);
     listdb_init(listdb);
}

/**
 * @brief Destroys a list database structure
 *
 * @param *listdb List database to be destroyed
 */
void listdb_destroy(ListDB *listdb)
{     
     int i;

     for (i = 0; i < listdb->size; i++)
          list_destroy(&listdb->lists[i]);

     free(listdb->lists);
     listdb_init(listdb);
}

/**
 * @brief Prints a database of lists
 *
 * @param listdb Database to be printed
 */
void listdb_print(ListDB *listdb)
{     
     uint i;
     for (i = 0; i < listdb->size; i++) {
          printf("[  %d  ] ", i);
          list_print(&listdb->lists[i]);
     }
}

/**
 * @brief Prints given positions of a database of lists
 *
 * @param listdb Database to be printed
 * @param positions Positions of the database to be printed
 */
void listdb_print_multi(ListDB *listdb, List *positions)
{
     uint i, pos;
     
     for (i = 0; i < positions->size; i++) {
          pos = positions->data[i].item;
          if (pos >= 0 && pos < listdb->size){
               printf("[  %d  ] ", pos);
               list_print(&listdb->lists[pos]);
          }
          else
               printf ("%d-OOR ", pos);
     }
     printf ("\n");
}

/**
 * @brief Prints a given range of a database of lists
 *
 * @param *listdb Database to be printed
 * @param low Start position of the range
 * @param high End position of the range
 */
void listdb_print_range(ListDB *listdb, uint low, uint high)
{     
     uint i;
     if (low >= 0 && low <= high && high < listdb->size) {
          printf ("%d -- %d-%d\n", listdb->size, low, high);
          for (i = low; i <= high; i++){
               printf("[  %d  ] ", i);
               list_print(&listdb->lists[i]); 
          }
     } else {
          printf ("Invalid range %d-%d. Database range is 0-%d", 
                  low, high, listdb->size - 1);
     }
}

/**
 * @brief List size comparison for bsearch and qsort. 
 *
 * @param *a First List to compare
 * @param *b Second List to compare
 *
 * @return 0 if the lists are equal, positive if the first list
 *         is greater than the second and negative otherwise.
 */
int listdb_size_compare(const void *a, const void *b)
{
     int a_size = ((List *)a)->size;
     int b_size = ((List *)b)->size;

     return a_size - b_size;
}

/**
 * @brief List size comparison for bsearch and qsort. 
 *
 * @param *a First List to compare
 * @param *b Second List to compare
 *
 * @return 0 if the lists are equal, positive if the second list
 *         is greater than the first and negative otherwise.
 */
int listdb_size_compare_back(const void *a, const void *b)
{
     int a_size = ((List *)a)->size;
     int b_size = ((List *)b)->size;

     return b_size - a_size;
}

/**
 * @brief Sorts a database of lists based on their size in ascending order
 *
 * @param *listdb Database to be sorted
 */
void listdb_sort_by_size(ListDB *listdb)
{
     qsort(listdb->lists, listdb->size, sizeof(List), listdb_size_compare);
}

/**
 * @brief Sorts a database of lists based on their size in ascending order
 *
 * @param *listdb Database to be sorted
 */
void listdb_sort_by_size_back(ListDB *listdb)
{
     qsort(listdb->lists, listdb->size, sizeof(List), listdb_size_compare_back);
}

/**
 * @brief Computes scores of lists
 *
 * @param *listdb Database to be sorted
 * @param *func Function for computing scores
 *
 * @return Computed scores
 */
Score *listdb_compute_scores(ListDB *listdb, double (*func)(List *))
{
     Score *scores = malloc(listdb->size);
     
     uint i;
     for (i = 0; i < listdb->size; i++) {
          scores[i].index = i;
          scores[i].value = func(&listdb->lists[i]);
     }
}

/**
 * @brief Swaps lists in a database based on their scores
 *
 * @param *listdb Database of lists
 * @param *scores Scores of the lists
 */
void listdb_swap_by_score(ListDB *listdb, Score *scores)
{
     ListDB newlistdb = listdb_create(listdb->size, listdb->dim);
     
     uint i;
     for (i = 0; i < listdb->size; i++) 
          newlistdb.lists[i] = listdb->lists[scores[i].index];
     
     listdb_clear(listdb);
     *listdb = newlistdb;
}

/**
 * @brief Sorts a database of lists based on their size in ascending order
 *
 * @param *listdb Database to be sorted
 */
void listdb_sort_by_score(ListDB *listdb, double (*func)(List *))
{
     Score *scores = listdb_compute_scores(listdb, func);
     qsort(scores, listdb->size, sizeof(Score), list_score_compare);	
     listdb_swap_by_score(listdb, scores);
}

/**
 * @brief Sorts a database of lists based on their size in ascending order
 *
 * @param *listdb Database to be sorted
 */
void listdb_sort_by_score_back(ListDB *listdb, double (*func)(List *))
{
     Score *scores = listdb_compute_scores(listdb, func);
     qsort(scores, listdb->size, sizeof(Score), list_score_compare_back);	
     listdb_swap_by_score(listdb, scores);
}

/**
 * @brief Applies a function to all the lists in a database
 *
 * @param *listdb Database where the function will be applied
 * @param *func Function to apply
 */
void listdb_apply_to_all(ListDB *listdb, void (*func)(List *))
{
     uint i;
     for (i = 0; i < listdb->size; i++)
          func(&listdb->lists[i]);
}

/**
 * @brief Applies a function to a range of lists in a database
 *
 * @param *listdb Database where the function will be applied
 * @param *func Function to apply
 * @param low Start of the range
 * @param high End of the range
 */
void listdb_apply_to_range(ListDB *listdb, void (*func)(List *), uint low,
                           uint high)
{
     uint i;
     if (low >= 0 && low <= high && high < listdb->size) {
          for (i = low; i <= high; i++)
               func(&listdb->lists[i]);
     } else {
          printf ("%d-%d-OOR\n", low, high);
     }
}

/**
 * @brief Applies a function to certain of lists in a database
 *
 * @param *listdb Database where the function will be applied
 * @param *func Function to apply
 * @param *positions Positions of the lists where the function will be applied
 */
void listdb_apply_to_multi(ListDB *listdb, void (*func)(List *), List *positions)
{
     uint i, pos;
     for (i = 0; i < positions->size; i++) {
          pos = positions->data[i].item;
          if (pos >= 0 && pos < listdb->size)
               func(&listdb->lists[pos]);
          else
               printf ("Position %d OOR\n", pos);
     }
}

/**
 * @brief Adds a new list to the end of a database
 *
 * @param *listdb Database where the list will be added
 * @param list list to be added
 */
void listdb_push(ListDB *listdb, List *list)
{
     uint newsize = listdb->size + 1;
     listdb->lists = realloc(listdb->lists, newsize * sizeof(List));
     listdb->lists[listdb->size] = *list;
     listdb->size = newsize;
}

/**
 * @brief Deletes the list at the end of a database
 *
 * @param *listdb Database where the list will be deleted
 */
void listdb_pop(ListDB *listdb)
{
     listdb->size--;
     list_destroy(&listdb->lists[listdb->size]);
     listdb->lists = realloc(listdb->lists, listdb->size * sizeof(List));
}

/**
 * @brief Deletes multiple lists at the end of a database
 *
 * @param *listdb List database where the lists will be deleted
 * @param number Number of lists to be removed
 */
void listdb_pop_multi(ListDB *listdb, uint number)
{
     listdb_apply_to_range(listdb, list_destroy, listdb->size - number - 1, listdb->size - 1);
     listdb->size -= number;
     listdb->lists = realloc(listdb->lists, listdb->size * sizeof(List));
}

/**
 * @brief Deletes multiple lists at the end of a database
 *
 * @param *listdb List database where the lists will be deleted
 * @param last Last list to be removed
 */
void listdb_pop_until(ListDB *listdb, uint last)
{
     listdb_apply_to_range(listdb, list_destroy, last, listdb->size - 1);
     listdb->size = last;
     listdb->lists = realloc(listdb->lists, listdb->size * sizeof(List));
}

/**
 * @brief Deletes a list in a given position in a database
 *
 * @param *listdb Database where the list will be deleted
 * @param position Position of the list to be deleted
 */
void listdb_delete_position(ListDB *listdb, uint position)
{
     list_destroy(&listdb->lists[position]);
     uint newsize = listdb->size - 1;
     List *tmplists = (List *) malloc(newsize * sizeof(List));
     memcpy(tmplists, listdb->lists, position * sizeof(List));
     memcpy(tmplists + position, listdb->lists + position + 1, 
            (newsize - position) * sizeof(List));
     free(listdb->lists);
     listdb->lists = tmplists;
     listdb->size = newsize;
}

/**
 * @brief Deletes lists in a given range of a database
 *
 * @param *listdb Database where the lists will be deleted
 * @param low Start position of the range
 * @param high End position of the range
 */
void listdb_delete_range(ListDB *listdb, uint low, uint high)
{    
     listdb_apply_to_range(listdb, list_destroy, low, high);
     uint range = high - low + 1;
     uint newsize = listdb->size - range;
     List *tmplists = (List *) malloc(newsize * sizeof(List));
     memcpy(tmplists, listdb->lists, low * sizeof(List));
     memcpy(tmplists + low, listdb->lists + high + 1, 
            (listdb->size - high - 1) * sizeof(List));
     free(listdb->lists);
     listdb->lists = tmplists;
     listdb->size = newsize;
}

/**
 * @brief Deletes lists smaller than a given size
 *
 * @param *listdb Database where the lists will be deleted
 * @param min_size Minimum size
 */
void listdb_delete_smallest(ListDB *listdb, uint min_size)
{
     int pos;

     listdb_sort_by_size_back(listdb);
     for (pos = 0; pos < listdb->size; pos++)
          if (listdb->lists[pos].size < min_size)
               break;

     if (pos < listdb->size) {
          uint i;
          for (i = pos; i < listdb->size; i++)
               list_destroy(&listdb->lists[i]);

          listdb_pop_until(listdb, pos);
     }
}

/**
 * @brief Deletes lists larger than a given size
 *
 * @param *listdb Database where the lists will be deleted
 * @param max_size Maximum size
 */
void listdb_delete_largest(ListDB *listdb, uint max_size)
{
     uint pos;

     listdb_sort_by_size(listdb);
     for (pos = 0; pos < listdb->size; pos++)
          if (listdb->lists[pos].size > max_size)
               break;

     if (pos < listdb->size) {
          uint i;
          for (i = pos; i < listdb->size; i++)
               list_destroy(&listdb->lists[i]);

          listdb_pop_until(listdb, pos);
     }
}

/**
 * @brief Inserts a list in a given position in a database
 *
 * @param *listdb Database where the list will be inserted
 * @param *list List to be inserted
 * @param position Position where the list is to be inserted
 */
void listdb_insert(ListDB *listdb, List *new_list, uint position)
{
     uint newsize = listdb->size + 1;
     List *tmplists;
     tmplists = (List *) malloc(newsize * sizeof(List));
     memcpy(tmplists, listdb->lists, position * sizeof(List));
     tmplists[position] = *new_list;
     if (position < listdb->size)
          memcpy(tmplists + position + 1, listdb->lists + position, 
                 (listdb->size - position) * sizeof(List));

     free(listdb->lists);
     listdb->lists = tmplists;
     listdb->size = newsize;
}

/**
 * @brief Appends one list database to another
 *
 * @param listdb1 Base list database
 * @param listdb2 List database to be appended
 */
void listdb_append(ListDB *listdb1, ListDB *listdb2)
{
     uint newsize = listdb1->size + listdb2->size;

     listdb1->lists = realloc(listdb1->lists, newsize * sizeof(List));
     memcpy(listdb1->lists + listdb1->size, listdb2->lists, listdb2->size * sizeof(List));
     listdb1->size = newsize;
}

/**
 * @brief Appends one list into another and deletes second list from database
 *
 * @param *listdb Database
 * @param position1 Position of the first list
 * @param position2 Position of the list to be appended
 */
void listdb_append_lists_delete(ListDB *listdb, uint position1, uint position2)
{
     list_append(&listdb->lists[position1], &listdb->lists[position2]);
     listdb_delete_position(listdb, position2);
}

/**
 * @brief Appends one list into another in a database and destroys second list
 *
 * @param *listdb Database
 * @param position1 Position of the first list
 * @param position2 Position of the list to be appended
 */
void listdb_append_lists_destroy(ListDB *listdb, uint position1, uint position2)
{
     list_append(&listdb->lists[position1], &listdb->lists[position2]);
     list_destroy(&listdb->lists[position2]);
}

/**
 * @brief Adds one list with another and deletes second list from database
 *
 * @param *listdb Database
 * @param position1 Position of the first list
 * @param position2 Position of the list to be added
 */
void listdb_add_lists_delete(ListDB *listdb, uint position1, uint position2)
{
     list_add(&listdb->lists[position1], &listdb->lists[position2]);
     listdb_delete_position(listdb, position2);
}

/**
 * @brief Adds one list with another and destroys second list
 *
 * @param *listdb Database
 * @param position1 Position of the first list
 * @param position2 Position of the list to be added
 */
void listdb_add_lists_destroy(ListDB *listdb, uint position1, uint position2)
{
     list_add(&listdb->lists[position1], &listdb->lists[position2]);
     list_destroy(&listdb->lists[position2]);
}

/**
 * @brief Loads a list database from a file
 *        Format: 
 *             size item_1:freq_1 item_2:freq_2 ... item_size:freq_size
 *                        ...
 *
 * @param filename File containing the inverted file index of lists
 *
 * @return List database
 */
ListDB listdb_load_from_file(char *filename)
{
     FILE *file;
     if (!(file = fopen(filename,"r"))) {
          fprintf(stderr,"Error: Could not open file %s\n", filename);
          exit(EXIT_FAILURE);
     }

     // checking the number of lists in the file
     size_t len = 0;
     ssize_t read;
     char *line = NULL;
     ListDB listdb;
     listdb.size = 0;
     while ((read = getline(&line, &len, file)) != -1) {
          if ('\n' != line[0])
               listdb.size++;
     }
     rewind(file);

     // reading lists
     listdb.dim = 0;
     listdb.lists = (List *) malloc(listdb.size * sizeof(List));

     uint i, j;
     for (i = 0; i < listdb.size; i ++) {
          fscanf(file,"%u", &listdb.lists[i].size);
          listdb.lists[i].data = (Item *) malloc(listdb.lists[i].size * sizeof(Item));
          for (j = 0; j < listdb.lists[i].size; j++) {
               char sep;
               fscanf(file,"%u%c%u", &listdb.lists[i].data[j].item, &sep, &listdb.lists[i].data[j].freq);
               if (listdb.dim < listdb.lists[i].data[j].item + 1)
                    listdb.dim = listdb.lists[i].data[j].item + 1;
          }
     }
     
     if (fclose(file)) {
          fprintf(stderr,"Error: Could not close file %s\n", filename);
          exit(EXIT_FAILURE);
     }
     
     return listdb;
}

/**
 * @brief Saves a list database in a file.
 *        Format: 
 *             size item_1:freq_1 item_2:freq_2 ... item_size:freq_size
 *
 * @param filename File where the inverted file index will be saved
 * @param listdb List database to save
 */
void listdb_save_to_file(char *filename, ListDB *listdb)
{
     FILE *file;     
     if (!(file = fopen(filename,"w"))) {
          fprintf(stderr,"Error: Could not create file %s\n", filename);
          exit(EXIT_FAILURE);
     }

     uint i, j;
     for (i = 0; i < listdb->size; i++) {
          fprintf(file,"%u", listdb->lists[i].size);
          for (j = 0; j < listdb->lists[i].size; j++)
               fprintf(file," %u:%u", listdb->lists[i].data[j].item, listdb->lists[i].data[j].freq);
          fprintf(file,"\n");
     }

     if (fclose(file)) {
          fprintf(stderr,"Error: Could not close file %s\n", filename);
          exit(EXIT_FAILURE);
     }
}
