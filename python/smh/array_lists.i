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
}Item;


typedef struct List{
     uint size;
     Item *data;
}List;

typedef unsigned int uint;

extern void list_init(List *);
extern List list_create(uint);
extern List list_random(uint, uint);
extern void list_destroy(List *);
Item list_make_item(uint, uint);
extern Item *list_find(List *, Item);
extern Item *list_min_item(List *);
extern Item *list_max_item(List *);
extern uint list_sum_freq(List *);
extern Item *list_binary_search(List *, Item);
extern void list_sort_by_item(List *);
extern void list_sort_by_item_back(List *);
extern void list_sort_by_frequency(List *);
extern void list_sort_by_frequency_back(List *);
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

%pythoncallback;
extern double list_jaccard(List *, List *);
extern double list_overlap(List *, List *);
%nopythoncallback;

%ignore list_jaccard;
%ignore list_overlap;

extern double list_histogram_intersection(List *, List *);
extern double list_weighted_histogram_intersection(List *, List *, uint *);
extern double list_weighted_similarity(List *, List *, uint *);

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
