/* listdb.i */
%module listdb
%include typemaps.i
%include "exception.i"
%{
#include "array_lists.h"
#include "listdb.h"
#include <assert.h>

     static int myErr = 0;

     %}
 
extern void listdb_init(ListDB *listdb);
extern ListDB listdb_create(int, int);
extern ListDB listdb_random(uint, uint, uint);
extern void listdb_clear(ListDB *listdb);
extern void listdb_destroy(ListDB *);
extern void listdb_print(ListDB *);
extern void listdb_print_multi(ListDB *, List *);
extern void listdb_print_range(ListDB *, uint, uint);
extern void listdb_delete_smallest(ListDB *, uint);
extern void listdb_delete_largest(ListDB *, uint);

extern ListDB listdb_load_from_file(char *filename);
extern void listdb_save_to_file(char *filename, ListDB *listdb);

typedef struct ListDB{
     uint size;
     uint dim;
     List *lists;
}ListDB;

typedef unsigned int uint;

%exception ListDB::__getitem__ {
     assert(!myErr);
     $action
          if (myErr) {
               myErr = 0; // clear flag for next time
               SWIG_exception(SWIG_IndexError, "Index out of bounds");
          }
}

%extend ListDB {
     uint number_of_items;
     List __getitem__(size_t i) {
          if (i >= $self->size) {
               myErr = 1;
               List list = (const List) {0};
               return list;
          }
          return $self->lists[i];
     }

     uint *compute_number_of_items(void) {
          uint i;
          uint number_of_items = 0;
          for (i = 0; i < $self->size; i++)
               number_of_items += $self->lists[i].size;
     }
     
     uint *rows(void) {
          uint i;
          uint *rows = (uint *) malloc($self->number_of_items * sizeof(uint));
          uint counter = 0;
          for (i = 0; i < $self->size; i++) {
               memset(&rows[counter], i, $self->lists[i].size);
               counter += $self->lists[i].size;
          }
     }

     uint *cols(void) {
          uint i;
          uint *cols = (uint *) malloc(number_of_items * sizeof(uint));
          uint counter = 0;
          for (i = 0; i < $self->size; i++) {
               for (j = 0; j < $self->lists[i].size; j++) {
               }
               counter += $self->lists[i].size;
                                   
          }
     }
}
