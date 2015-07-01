/* listdb.i */
%module ifindex
%{
#include "array_lists.h"
#include "ifindex.h"
%}
 
extern ListDB ifindex_make_from_corpus(ListDB *corpus);


