/**
 * @file array_lists.c
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
 * @brief Basic operations on array lists.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <float.h>
#include "array_lists.h"

/**
 * @brief Initializes a list
 *
 * @param list List to be initialized
 */
void list_init(List *list)
{
     list->size = 0;
     list->data = NULL;
}

/**
 * @brief Creates a list of a given size with items initialized to zero
 *
 * @param size Size of the list to be created
 *
 * @return Created list
 */
List list_create(uint size)
{
     List list;

     list.size = size;
     list.data = (Item *) calloc(size, sizeof(Item));

     return list;
}

/**
 * @brief Creates a random (uniformly distributed) list
 *
 * @param max_size Maximum size of the list
 * @param max_item Maximum value of items
 *
 * @return Random list
 */
List list_random(uint max_size, uint max_item)
{
     uint i;
     uint random_size = rand() % max_size;
     List list;
     Item random_item;

     list_init(&list);
     for (i = 0; i < random_size; i++) {
          random_item.item = rand() % max_item;
          random_item.freq = 1;
          list_push(&list, random_item);
     }

     return list;
}

/**
 * @brief Destroys a list
 *
 * @param list List to be destroyed
 */
void list_destroy(List *list)
{
     free(list->data);
     list_init(list);
}

/**
 * @brief Makes an item
 *
 * @param ID of the item to be created
 * @param Frequency of the item to be created
 *
 * @return Created item
 */
Item list_make_item(uint id, uint freq)
{
     Item item = {id, freq};
     
     return item;
}

/**
 * @brief Searches for the first occurrence of an item in an unsorted list
 *
 * @param list List where the item is to be searched
 * @param item Item to be searched
 *
 * @return Pointer to the found item
 */
Item *list_find(List *list, Item item)
{
     uint i;

     for (i = 0; i < list->size; i++)
          if (list->data[i].item == item.item)
               return list->data + i;

     return NULL;
}

/**
 * @brief Finds minimum value of an unordered list
 *
 * @param list List where the minimum item is to be searched
 *
 * @return Pointer to the minimum item
 */
Item *list_min_item(List *list)
{
     uint i;
     Item *min_item;

     min_item = list->data;
     for (i = 1; i < list->size; i++)
          if (min_item->item > list->data[i].item)
               min_item = list->data + i;

     return min_item;
}

/**
 * @brief Finds maximum value of an unordered list
 *
 * @param list List where the maximum item is to be searched
 *
 * @return Pointer to the maximum item
 */
Item *list_max_item(List *list)
{
     uint i;
     Item *max_item;

     max_item = list->data;
     for (i = 1; i < list->size; i++)
          if (max_item->item < list->data[i].item)
               max_item = list->data + i;

     return max_item;
}

/**
 * @brief Finds the item with the minimum frequency of an unordered list
 *
 * @param list List where the item with the minimum frequency is to be searched
 *
 * @return Pointer to the item with the minimum frequency
 */
Item *list_min_freq(List *list)
{
     uint i;
     Item *min_freq;

     min_freq = list->data;
     for (i = 1; i < list->size; i++)
          if (min_freq->freq > list->data[i].freq)
               min_freq = list->data + i;

     return min_freq;
}

/**
 * @brief Finds maximum frequency of an unordered list
 *
 * @param list List where the item with the maximum frequency is to be searched
 *
 * @return Pointer to the item with the maximum frequency
 */

Item *list_max_freq(List *list)
{
     uint i;
     Item *max_freq;

     max_freq = list->data;
     for (i = 1; i < list->size; i++)
          if (max_freq->freq < list->data[i].freq)
               max_freq = list->data + i;

     return max_freq;
}

/**
 * @brief Computes the sum of the frequencies of a list
 *
 * @param list List from which the sum of frequencies is to be computed
 *
 * @return Sum of the frequencies of the input list
 */
uint list_sum_freq(List *list)
{
     uint i;
     uint sum = 0;

     for (i = 0; i < list->size; i++)
          sum += list->data[i].freq;
              
     return sum;
}

/**
 * @brief List item comparison for bsearch and qsort.
 *
 * @param a First item to compare
 * @param b Second item to compare
 *
 * @return 0 if the items are equal, positive if the first item
 *         is greater than the second and negative otherwise.
 */
int list_item_compare(const void *a, const void *b)
{
     int a_item = ((Item *)a)->item;
     int b_item = ((Item *)b)->item;

     return a_item - b_item;
}

/**
 * @brief List item comparison for bsearch and qsort.
 *
 * @param a First item to compare
 * @param b Second item to compare
 *
 * @return 0 if the items are equal, positive if the second item
 *         is greater than the first and negative otherwise.
 */
int list_item_compare_back(const void *a, const void *b)
{
     int a_item = ((Item *)a)->item;
     int b_item = ((Item *)b)->item;

     return b_item - a_item;
}

/**
 * @brief List frequency comparison for bsearch and qsort.
 *
 * @param a First item to compare
 * @param b Second item to compare
 *
 * @return 0 if the items are equal, positive if the frequency of first item
 *         is greater than the second and negative otherwise.
 */
int list_frequency_compare(const void *a, const void *b)
{
     int a_freq = ((Item *)a)->freq;
     int b_freq = ((Item *)b)->freq;

     return a_freq - b_freq;
}

/**
 * @brief List frequency comparison for bsearch qsort.
 *
 * @param a First item to compare
 * @param b Second item to compare
 *
 * @return 0 if the items are equal, positive if the frequency of second item
 *         is greater than the first and negative otherwise.
 */
int list_frequency_compare_back(const void *a, const void *b)
{
     int a_freq = ((Item *)a)->freq;
     int b_freq = ((Item *)b)->freq;

     return b_freq - a_freq;
}

/**
 * @brief Score comparison for bsearch and qsort. 
 *
 * @param *a First score to compare
 * @param *b Second score to compare
 *
 * @return 0 if the scores are equal, positive if the first score
 *         is greater than the second and negative otherwise.
 */
int list_score_compare(const void *a, const void *b)
{
     double a_val = ((Score *)a)->value;
     double b_val = ((Score *)b)->value;

     return a_val - b_val;
}

/**
 * @brief Score comparison for bsearch and qsort. 
 *
 * @param *a First score to compare
 * @param *b Second score to compare
 *
 * @return 0 if the scores are equal, positive if the second score
 *         is greater than the first and negative otherwise.
 */
int list_score_compare_back(const void *a, const void *b)
{
     double a_val = ((Score *)a)->value;
     double b_val = ((Score *)b)->value;

     return b_val - a_val;
}

/**
 * @brief Searches for an item in a sorted list using the binary search algorithm.
 *
 * @param list List where the item is to be searched
 * @param item Item to be searched
 *
 * @return Pointer to the found item
 */
Item *list_binary_search(List *list, Item query)
{
     return bsearch(&query, list->data, list->size, sizeof(Item), list_item_compare);
}

/**
 * @brief Sorts a list based on item values in ascending order
 *
 * @param list List to be sorted
 */
void list_sort_by_item(List *list)
{
     qsort(list->data, list->size, sizeof(Item), list_item_compare);
}

/**
 * @brief Sorts a list based on item values in descending order
 *
 * @param list List to be sorted
 */
void list_sort_by_item_back(List *list)
{
     qsort(list->data, list->size, sizeof(Item), list_item_compare_back);
}

/**
 * @brief Sorts a list based on items frequency in ascending order
 *
 * @param list List to be sorted
 */
void list_sort_by_frequency(List *list)
{
     qsort(list->data, list->size, sizeof(Item), list_frequency_compare);
}

/**
 * @brief Sorts a list based on items frequency in descending order
 *
 * @param list List to be sorted
 */
void list_sort_by_frequency_back(List *list)
{
     qsort(list->data, list->size, sizeof(Item), list_frequency_compare_back);
}

/**
 * @brief Prints in screen the items of a list
 *
 * @param list List to be printed
 */
void list_print(List *list)
{
     uint i;

     printf ("%d -- ", list->size);
     for (i = 0; i < list->size; i++)
          printf ("%d:%d[%d] ", list->data[i].item, list->data[i].freq, i);
     printf("\n");
}

/**
 * @brief Prints in screen the items of a list specified by another list
 *
 * @param list List to be printed
 * @param positions Positions of the list items to be printed
 */
void list_print_multi(List *list, List *positions)
{
     uint i, pos;

     printf ("%d -- ", positions->size);
     for (i = 0; i < positions->size; i++) {
          pos = positions->data[i].item;
          if (pos >= 0 && pos < list->size)
               printf ("%d:%d[%d]", list->data[pos].item, list->data[pos].freq, pos);
          else
               printf ("%d-OOR ", pos);
     }
     printf ("\n");
}

/**
 * @brief Prints in screen the items in a given list range
 *
 * @param list List to be printed
 * @param low Start position of the range
 * @param high End position of the range
 */
void list_print_range(List *list, uint low, uint high)
{
     uint i;

     if (low >= 0 && low <= high && high < list->size) {
          printf ("%d (%d-%d) -- ", list->size, low, high);
          for (i = low; i <= high; i++)
               printf ("%d:%d[%d] ", list->data[i].item, list->data[i].freq, i);
     } else {
          printf ("Invalid range %d-%d. List range is 0-%d", low, high,
                  list->size - 1);
     }
     printf ("\n");
}

/**
 * @brief Adds a new item to the end of a list
 *
 * @param list List where the item will be added
 * @param item Item to be added
 */
void list_push(List *list, Item item)
{
     uint newsize = list->size + 1;

     list->data = realloc(list->data, newsize * sizeof(Item));
     list->data[list->size] = item;
     list->size = newsize;
}

/**
 * @brief Adds items from a range of a given list to the end of another list
 *
 * @param list List where the items will be added
 * @param items List from which items will be added
 * @param low Start position of the range for the list of items to be added
 * @param high End position of the range for the list of items to be added
 */
void list_push_range(List *list, List *items, uint low, uint high)
{
     uint range = high - low + 1;
     uint newsize = list->size + range;

     list->data = realloc(list->data, newsize * sizeof(Item));
     memcpy(list->data + list->size, items->data + low, range * sizeof(Item));
     list->size = newsize;
}

/**
 * @brief Removes the item at the end of a list
 *
 * @param list List where the item will be removed
 */
void list_pop(List *list)
{
     list->size--;
     list->data = realloc(list->data, list->size * sizeof(Item));
}

/**
 * @brief Removes multiple items at the end of a list
 *
 * @param list List where the items will be removed
 * @param number Number of items to be removed
 */
void list_pop_multi(List *list, uint number)
{
     list->size -= number;
     list->data = realloc(list->data, list->size * sizeof(Item));
}

/**
 * @brief Removes all items at the end of a list from a given position
 *
 * @param list List where the items will be removed
 * @param last Position from which the rest of the items will be removed
 */
void list_pop_until(List *list, uint last)
{
     list->size = last;
     list->data = realloc(list->data, list->size * sizeof(Item));
}

/**
 * @brief Deletes an item in a given position in a list
 *
 * @param list List where the item will be deleted
 * @param position Position of the item to be deleted
 */
void list_delete_position(List *list, uint position)
{
     uint newsize = list->size - 1;
     Item *tmpdata = (Item *) malloc(newsize * sizeof(Item));

     memcpy(tmpdata, list->data, position * sizeof(Item));
     memcpy(tmpdata + position, list->data + position + 1,
            (newsize - position) * sizeof(Item));
     free(list->data);
     list->data = tmpdata;
     list->size = newsize;
}

/**
 * @brief Deletes a specific item from a list
 *
 * @param list List where the item will be deleted
 * @param item Item to be deleted
 */
void list_delete_item(List *list, Item item)
{
     Item *found = list_binary_search(list, item);

     if (found != NULL) {
          uint position = (uint)(found - list->data);
          uint newsize = list->size - 1;
          Item *tmpdata = (Item *) malloc(newsize * sizeof(Item));

          memcpy(tmpdata, list->data, position * sizeof(Item));
          memcpy(tmpdata + position, list->data + position + 1,
                 (list->size - position - 1) * sizeof(Item));
          free(list->data);
          list->data = tmpdata;
          list->size = newsize;
     }
}

/**
 * @brief Deletes items in a given position range of a list
 *
 * @param list List where the items will be deleted
 * @param low Start position of the range
 * @param high End position of the range
 */
void list_delete_range(List *list, uint low, uint high)
{
     uint range = high - low + 1;
     uint newsize = list->size - range;
     Item *tmpdata = (Item *) malloc(newsize* sizeof(Item));

     memcpy(tmpdata, list->data, low * sizeof(Item));
     memcpy(tmpdata + low, list->data + high + 1,
            (list->size - high - 1) * sizeof(Item));
     free(list->data);
     list->data = tmpdata;
     list->size = newsize;
}

/**
 * @brief Deletes lists less frequent than a given value
 *
 * @param list List where the items will be deleted
 * @param min_freq Minimum frequency
 */
void list_delete_less_frequent(List *list, uint min_freq)
{
     int pos;

     list_sort_by_frequency_back(list);
     for (pos = 0; pos < list->size; pos++)
          if (list->data[pos].freq < min_freq)
               break;

     if (pos < list->size)
          list_pop_until(list, pos);
}

/**
 * @brief Deletes lists more frequent than a given value
 *
 * @param list List where the items will be deleted
 * @param max_freq Maximum frequency
 */
void list_delete_more_frequent(List *list, uint max_freq)
{
     int pos;

     list_sort_by_frequency(list);
     for (pos = 0; pos < list->size; pos++)
          if (list->data[pos].freq > max_freq)
               break;

     if (pos < list->size)
          list_pop_until(list, pos);
}

/**
 * @brief Deletes repeated items in a list
 *
 * @param list List where the repeated items will be deleted
 */
void list_unique(List *list)
{
     uint i;

     if (list->size > 0) {
          for (i = 0; i < list->size - 1; i++) {
               if (list->data[i].item == list->data[i + 1].item) {
                    int k = 1;
                    while(i + k < list->size &&
                          list->data[i].item == list->data[i + k].item) {
                         list->data[i].freq += list->data[i + k].freq;
                         k++;
                    }
                    list_delete_range(list, i + 1, i + k - 1);
               }
          }
     }
}

/**
 * @brief Inserts an item in a given position in a list
 *
 * @param list List where the item will be inserted
 * @param item Item to be inserted
 * @param position Position where the item is to be inserted
 */
void list_insert(List *list, Item item, uint position)
{
     uint newsize = list->size + 1;
     Item *tmpdata;

     tmpdata = (Item *) malloc(newsize * sizeof(Item));
     memcpy(tmpdata, list->data, (position + 1) * sizeof(Item));
     tmpdata[position] = item;
     memcpy(tmpdata + position + 1, list->data + position,
            (list->size - position) * sizeof(Item));
     free(list->data);
     list->data = tmpdata;
     list->size = newsize;
}

/**
 * @brief Duplicates a list
 *
 * @param list List to be duplicated
 *
 * @return Duplicated list
 */
List list_duplicate(List *src)
{
     List duplicate;

     duplicate.data = (Item *) malloc(src->size * sizeof(Item));
     memcpy(duplicate.data, src->data, src->size * sizeof(Item));
     duplicate.size = src->size;

     return duplicate;
}

/**
 * @brief Copies a range of a list into another list
 *
 * @param src Source list
 * @param low Start position of the range
 * @param high End position of the range
 *
 * @return Copy of the list
 */
List list_copy_range(List *src, uint low, uint high)
{
     List copy;
     uint range = high - low + 1;

     copy.data = (Item *) malloc(range * sizeof(Item));
     memcpy(copy.data, src->data + low, range * sizeof(Item));
     copy.size = range;

     return copy;
}

/**
 * @brief Concatenation of a pair of lists
 *
 * @param list1 First list to concatenate
 * @param list2 Second list to concatenate
 *
 * @return Concatenation list
 */
List list_concat(List *list1, List *list2)
{
     uint newsize = list1->size + list2->size;
     List concat;

     concat.data = (Item *) malloc(newsize * sizeof(Item));
     memcpy(concat.data, list1->data, list1->size * sizeof(Item));
     memcpy(concat.data + list1->size, list2->data, list2->size * sizeof(Item));
     concat.size = newsize;

     return concat;
}

/**
 * @brief Appends one list to another
 *
 * @param list1 Base list
 * @param list2 List to be appended
 */
void list_append(List *list1, List *list2)
{
     uint newsize = list1->size + list2->size;

     list1->data = realloc(list1->data, newsize * sizeof(Item));
     memcpy(list1->data + list1->size, list2->data, list2->size * sizeof(Item));
     list1->size = newsize;
}

/**
 * @brief Appends one list to another and removes duplicated items
 *
 * @param list1 Base list
 * @param list2 List to be added
 */
void list_add(List *list1, List *list2)
{
     list_append(list1, list2);
     list_sort_by_item(list1);
     list_unique(list1);
}

/**
 * @brief Computes the union list from two lists
 *
 * @param list1 First list
 * @param list2 Second list
 *
 * @return Union list
 */
List list_union(List *list1, List *list2)
{
     uint i = 0, j = 0;
     List union_list;

     list_init(&union_list);
     while (i < list1->size && j < list2->size) {
          if (list1->data[i].item == list2->data[j].item) {
               Item new_item;
               new_item.item = list1->data[i].item;
               new_item.freq = max(list1->data[i].freq, list2->data[j].freq);
               list_push(&union_list, new_item);
               i++;
               j++;
          } else if (list1->data[i].item < list2->data[j].item) {
               Item new_item;
               new_item.item = list1->data[i].item;
               new_item.freq = list1->data[i].freq;
               list_push(&union_list, new_item);
               i++;
          } else {
               Item new_item;
               new_item.item = list2->data[j].item;
               new_item.freq = list2->data[j].freq;
               list_push(&union_list, new_item);
               j++;
          }
     }

     while (i < list1->size) {
          Item new_item;
          new_item.item = list1->data[i].item;
          new_item.freq = list1->data[i].freq;
          list_push(&union_list, new_item);
          i++;
     }

     while (j < list2->size) {
          Item new_item;
          new_item.item = list2->data[j].item;
          new_item.freq = list2->data[j].freq;
          list_push(&union_list, new_item);
          j++;
     }

     return union_list;
}

/**
 * @brief Computes the union size of a pair of lists
 *
 * @param list1 First list
 * @param list2 Second list
 *
 * @return Union size
 */
uint list_union_size(List *list1, List *list2)
{
     uint i = 0, j = 0;
     uint union_size = 0;

     while (i < list1->size && j < list2->size) {
          if (list1->data[i].item == list2->data[j].item) {
               union_size++;
               i++;
               j++;
          } else if (list1->data[i].item < list2->data[j].item) {
               union_size++;
               i++;
          } else {
               union_size++;
               j++;
          }
     }

     while (i < list1->size) {
          union_size++;
          i++;
     }

     while (j < list2->size) {
          union_size++;
          j++;
     }

     return union_size;
}

/**
 * @brief Computes the intersection list from two lists
 *
 * @param list1 First list
 * @param list2 Second list
 *
 * @return Intersection list
 */
List list_intersection(List *list1, List *list2)
{
     uint i = 0, j = 0;
     List intersection_list;

     list_init(&intersection_list);
     while (i < list1->size && j < list2->size) {
          if (list1->data[i].item == list2->data[j].item) {
               Item new_item;
               new_item.item = list1->data[i].item;
               new_item.freq = min(list1->data[i].freq, list2->data[j].freq);
               list_push(&intersection_list, new_item);
               i++;
               j++;
          } else if (list1->data[i].item < list2->data[j].item) {
               i++;
          } else {
               j++;
          }
     }

     return intersection_list;
}

/**
 * @brief Computes the size of the intersection of a pair of lists
 *
 * @param list1 First list
 * @param list2 Second list
 *
 * @return Size of the intersection
 */
uint list_intersection_size(List *list1, List *list2)
{
     uint i = 0, j = 0;
     uint intersection_size = 0;

     while (i < list1->size && j < list2->size) {
          if (list1->data[i].item == list2->data[j].item) {
               intersection_size++;
               i++;
               j++;
          } else if (list1->data[i].item < list2->data[j].item) {
               i++;
          } else {
               j++;
          }
     }

     return intersection_size;
}

/**
 * @brief Computes the difference of a pair of lists
 *
 * @param list1 First list
 * @param list2 Second list
 *
 * @return Difference list
 */
List list_difference(List *list1, List *list2)
{
     uint i = 0, j = 0;
     List difference_list;

     list_init(&difference_list);
     while (i < list1->size && j < list2->size) {
          if (list1->data[i].item == list2->data[j].item) {
               i++;
               j++;
          } else if (list1->data[i].item > list2->data[j].item){
               j++;
          } else {
               Item new_item;
               new_item.item = list1->data[i].item;
               new_item.freq = list1->data[i].freq;
               list_push(&difference_list, new_item);
               i++;
          }
     }

     return difference_list;
}

/**
 * @brief Computes the size of the difference of a pair of lists
 *
 * @param list1 First list
 * @param list2 Second list
 *
 * @return Size of the difference list
 */
uint list_difference_size(List *list1, List *list2)
{
     uint i = 0, j = 0;
     uint difference_size = 0;

     while (i < list1->size && j < list2->size) {
          if (list1->data[i].item == list2->data[j].item) {
               i++;
               j++;
          } else if (list1->data[i].item > list2->data[j].item){
               j++;
          }
          else {
               difference_size++;
               i++;
          }
     }

     return difference_size;
}

/**
 * @brief Computes the jaccard similarity coefficient of a pair of lists.
 *
 * @param list1 First list
 * @param list2 Second list
 *
 * @return Jaccard similarity coefficient between lists
 */
double list_jaccard(List *list1, List *list2)
{
     if (list1->size > 0 && list2->size > 0){
          uint intersection_size = list_intersection_size(list1, list2);
          uint union_size = (list1->size + list2->size) - intersection_size;
          return (double) intersection_size / (double) union_size;
     } else {
          return 0.0;
     }
}

/**
 * @brief Computes the overlap coefficient of a pair of lists.
 *
 * @param list1 First list
 * @param list2 Second list
 *
 * @return Overlap coefficient between lists
 */
double list_overlap(List *list1, List *list2)
{

     if (list1->size > 0 && list2->size > 0){
          uint intersection_size = list_intersection_size(list1, list2);
          uint min_size = min(list1->size, list2->size);
          return (double) intersection_size / min_size;
     } else {
          return 0.0;
     }
}

/**
 * @brief Computes the weighted similarity of a pair of lists
 *
 * @param list1 First list
 * @param list2 Second list
 *
 * @return Weighted list similarity
 */
double list_weighted_similarity(List *list1, List *list2, double *weights)
{
     uint i = 0, j = 0;
     double weighted_inter = 0.0;
     double weighted_union = 0.0;

     if (list1->size > 0 && list2->size > 0) {
          while (i < list1->size && j < list2->size) {
               if (list1->data[i].item == list2->data[j].item) {
                    weighted_inter += weights[list1->data[i].item];
                    weighted_union += weights[list1->data[i].item];
                    i++;
                    j++;
               } else if (list1->data[i].item < list2->data[j].item) {
                    weighted_union += weights[list1->data[i].item];
                    i++;
               } else {
                    weighted_union += weights[list2->data[j].item];
                    j++;
               }
          }

          while (i < list1->size) {
               weighted_union += weights[list1->data[i].item];
               i++;
          }

          while (j < list2->size) {
               weighted_union += weights[list2->data[j].item];
               j++;
          }

          return weighted_inter / weighted_union;
     } else {
          return 0.0;
     }
}

/**
 * @brief Computes the histogram intersection of a pair of lists
 *
 * @param list1 First list
 * @param list2 Second list
 *
 * @return Histogram intersection
 */
double list_histogram_intersection(List *list1, List *list2)
{
     uint i = 0, j = 0;
     uint hist_inter = 0;
     uint hist_union = 0;

     if (list1->size > 0 && list2->size > 0) {
          while (i < list1->size && j < list2->size) {
               if (list1->data[i].item == list2->data[j].item) {
                    hist_inter += min(list1->data[i].freq, list2->data[j].freq);
                    hist_union += max(list1->data[i].freq, list2->data[j].freq);
                    i++;
                    j++;
               } else if (list1->data[i].item < list2->data[j].item) {
                    hist_union += list1->data[i].freq;
                    i++;
               } else {
                    hist_union += list2->data[j].freq;
                    j++;
               }
          }

          while (i < list1->size) {
               hist_union += list1->data[i].freq;
               i++;
          }

          while (j < list2->size) {
               hist_union += list2->data[j].freq;
               j++;
          }

          return (double) hist_inter / (double) hist_union;
     } else {
          return 0.0;
     }
}

/**
 * @brief Computes the weighted histogram intersection of a pair of lists
 *
 * @param list1 First list
 * @param list2 Second list
 *
 * @return Weighted histogram intersection
 */
double list_weighted_histogram_intersection(List *list1, List *list2, double *weights)
{
     uint i = 0, j = 0;
     double whist_inter = 0.0;
     double whist_union = 0.0;

     if (list1->size > 0 && list2->size > 0) {
          while (i < list1->size && j < list2->size) {
               if (list1->data[i].item == list2->data[j].item) {
                    double max_freq = (double) max(list1->data[i].freq, list2->data[j].freq);
                    double min_freq = (double) min(list1->data[i].freq, list2->data[j].freq);
                    whist_inter += weights[list1->data[i].item] * min_freq;
                    whist_union += weights[list1->data[i].item] * max_freq;
                    i++;
                    j++;
               } else if (list1->data[i].item < list2->data[j].item) {
                    whist_union += weights[list1->data[i].item] * list1->data[i].freq;
                    i++;
               } else {
                    whist_union += weights[list2->data[j].item] * list2->data[j].freq;
                    j++;
               }
          }

          while (i < list1->size) {
               whist_union += weights[list1->data[i].item] * list1->data[i].freq;
               i++;
          }

          while (j < list2->size) {
               whist_union += weights[list2->data[j].item] * list2->data[j].freq;
               j++;
          }

          return whist_inter / whist_union;
     } else {
          return 0.0;
     }
}

/**
 * @brief Checks if two lists are identical
 *
 * @param list1 First list
 * @param list2 Second list
 *
 * @return 1 if lists are identical, 0 otherwise
 */
uint list_equal(List *list1, List *list2)
{
     if (list1->size == list2->size) {
          uint i;
          uint equal = 1;
          for (i = 0; i < list1->size; i++) {
               if (list1->data[i].item != list2->data[i].item) {
                    equal = 0;
                    break;
               }
          }
          
          return equal;
     } else {
          return 0;
     }
}
