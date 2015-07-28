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
    List __getitem__(size_t i) {
        if (i >= $self->size) {
            myErr = 1;
            List list = (const List) {0};
            return list;
        }
        return $self->lists[i];
    }
}
