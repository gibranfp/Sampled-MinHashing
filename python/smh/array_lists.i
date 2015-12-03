/* array_lists.i */
%module array_lists
%include typemaps.i
%include "exception.i"
%{
     #include "array_lists.h"
     #include <assert.h>
     static int errItem = 0;
%}


typedef struct Item {
     uint item;
     uint freq;
} Item;


typedef struct List{
     uint size;
     Item *data;
} List;

typedef unsigned int uint;

%pythoncallback;
extern double list_jaccard(List *, List *);
extern double list_overlap(List *, List *);
extern void list_sort_by_frequency_back(List *);
extern void list_init(List *);
extern List list_create(uint);
extern List list_random(uint, uint);
extern void list_destroy(List *);
extern Item list_make_item(uint, uint);
extern Item *list_find(List *, Item);
extern Item *list_min_item(List *);
extern Item *list_max_item(List *);
extern uint list_sum_freq(List *);
extern Item *list_binary_search(List *, Item);
extern void list_sort_by_item(List *);
extern void list_sort_by_item_back(List *);
extern void list_sort_by_frequency(List *);
extern void list_print(List *);
extern void list_print_multi(List *, List *);
extern void list_print_range(List *, uint, uint);
extern void list_push(List *, Item);
extern void list_push_range(List *, List *, uint, uint);
extern void list_pop(List *);
extern void list_pop_multi(List *, uint);
extern void list_pop_until(List *, uint);
extern void list_delete_position(List *, uint);
extern void list_delete_item(List *, Item);
extern void list_delete_range(List *, uint, uint);
extern void list_delete_less_frequent(List *, uint);
extern void list_delete_more_frequent(List *, uint);
extern void list_unique(List *);
extern void list_insert(List *, Item, uint);
extern List list_duplicate(List *);
extern List list_copy_range(List *, uint, uint);
extern List list_concat(List *, List *);
extern void list_append(List *, List *);
extern void list_add(List *, List *);
extern List list_union(List *, List *);
extern uint list_union_size(List *, List *);
extern List list_intersection(List *, List *);
extern uint list_intersection_size(List *, List *);
extern List list_difference(List *, List *);
extern uint list_difference_size(List *, List *);
%nopythoncallback;

%ignore list_jaccard;
%ignore list_overlap;
%ignore list_sort_by_frequency_back;
%ignore list_init;
%ignore list_create;
%ignore list_random;
%ignore list_destroy;
%ignore list_make_item;
%ignore *list_find;
%ignore *list_min_item;
%ignore *list_max_item;
%ignore list_sum_freq;
%ignore *list_binary_search;
%ignore list_sort_by_item;
%ignore list_sort_by_item_back;
%ignore list_sort_by_frequency;
%ignore list_print;
%ignore list_print_multi;
%ignore list_print_range;
%ignore list_push;
%ignore list_push_range;
%ignore list_pop;
%ignore list_pop_multi;
%ignore list_pop_until;
%ignore list_delete_position;
%ignore list_delete_item;
%ignore list_delete_range;
%ignore list_delete_less_frequent;
%ignore list_delete_more_frequent;
%ignore list_unique;
%ignore list_insert;
%ignore list_duplicate;
%ignore list_copy_range;
%ignore list_concat;
%ignore list_append;
%ignore list_add;
%ignore list_union;
%ignore list_union_size;
%ignore list_intersection;
%ignore list_intersection_size;
%ignore list_difference;
%ignore list_difference_size;
%ignore list_weighted_similarity;
%ignore list_histogram_intersection;
%ignore list_weighted_histogram_intersection;

%exception List::__getitem__ {
    assert(!errItem);
    $action
    if (errItem) {
        errItem = 0; // clear flag for next time
        SWIG_exception(SWIG_IndexError, "Index out of bounds");
        }
    }

%extend List {
    Item  __getitem__(size_t i) {
        if (i >= $self->size) {
            errItem = 1;
            Item item = (const Item) {0};
            return item;
        }
        return $self->data[i];
    }
}
