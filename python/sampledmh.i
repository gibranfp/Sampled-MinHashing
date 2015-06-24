/* listdb.i */
%module sampledmh
%{
#include "array_lists.h"
#include "listdb.h"
#include "sampledmh.h"
extern ListDB sampledmh_mine(ListDB *listdb, uint tuple_size, uint number_of_tuples, uint table_size);
%}

extern ListDB sampledmh_mine(ListDB *listdb, int tuple_size, int number_of_tuples, int table_size);
 

