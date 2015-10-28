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
 
extern void listdb_init(ListDB *);
extern ListDB listdb_create(int, int);
extern ListDB listdb_random(uint, uint, uint);
extern void listdb_clear(ListDB *);
extern void listdb_destroy(ListDB *);
extern void listdb_push(ListDB *listdb, List *list);
extern void listdb_print(ListDB *);
extern void listdb_print_multi(ListDB *, List *);
extern void listdb_print_range(ListDB *, uint, uint);
extern void listdb_delete_smallest(ListDB *, uint);
extern void listdb_delete_largest(ListDB *, uint);
extern ListDB listdb_load_from_file(char *);
extern void listdb_save_to_file(char *, ListDB *);
extern void listdb_apply_to_all(ListDB *, void (*)(List *));

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
     List __getitem__(uint pos) {
          if (pos >= $self->size) {
               myErr = 1;
               List list = (const List) {0};
               return list;
          }
          
          return $self->lists[pos];
     }

     void push(uint pos, uint id, uint freq) {
          if (pos >= $self->size) {
               myErr = 1;
          } else{
               Item item = {id, freq};
               list_push($self->lists + pos, item);
          }
     }

     void push(uint pos, Item item) {
          if (pos >= $self->size) {
               myErr = 1;
          } else{
               list_push($self->lists + pos, item);
          }
     }
}
