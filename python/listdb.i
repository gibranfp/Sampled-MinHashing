/* listdb.i */
%module listdb
%{
#include "array_lists.h"
#include "listdb.h"
%}
 
extern void listdb_init(ListDB *listdb);
extern ListDB listdb_create(uint, uint);
extern ListDB listdb_random(uint, uint, uint);
extern void listdb_clear(ListDB *listdb);
extern void listdb_destroy(ListDB *);
extern void listdb_print(ListDB *);
extern void listdb_print_multi(ListDB *, List *);
extern void listdb_print_range(ListDB *, uint, uint);

extern ListDB listdb_load_from_file(char *filename);
extern void listdb_save_to_file(char *filename, ListDB *listdb);

typedef struct ListDB{
     uint size;
     uint dim;
     List *lists;
}ListDB;


