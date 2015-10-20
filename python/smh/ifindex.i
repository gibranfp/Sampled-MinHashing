/* ifindex.i */
%module ifindex
%{
#include "ifindex.h"
%}
 
extern ListDB ifindex_make_from_corpus(ListDB *corpus);


