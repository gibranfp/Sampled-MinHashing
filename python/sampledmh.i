/* listdb.i */
%module sampledmh
%{
#include "listdb.h"
#include "sampledmh.h"

#define DEF_TABLE_SIZE 524288
extern ListDB sampledmh_mine(ListDB *listdb, uint tuple_size, uint number_of_tuples, uint table_size);
%}

extern ListDB sampledmh_mine(ListDB *listdb, int tuple_size, int number_of_tuples, int table_size=DEF_TABLE_SIZE);
 

