/* listdb.i */
%module listdb
%{
#include "array_lists.h"
#include "listdb.h"
%}
 
extern void listdb_init(ListDB *listdb);
extern ListDB listdb_load_from_file(char *filename);
extern void listdb_save_to_file(char *filename, ListDB *listdb);

typedef struct ListDB{
     uint size;
     uint dim;
     List *lists;
}ListDB;


