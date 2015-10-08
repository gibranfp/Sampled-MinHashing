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

extern void listdb_delete_largest(ListDB *, uint);
extern uint list_intersection_size(List *, List *);
extern List* list_sum_freq(List *);

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
