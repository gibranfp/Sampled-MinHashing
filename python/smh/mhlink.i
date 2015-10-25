/* mhlink.i */
%module mhlink

%{
#include "mhlink.h"
%}
 
extern ListDB mhlink_cluster(ListDB *, uint, uint, uint, double (*)(List *, List *), double, uint);
extern ListDB mhlink_make_model(ListDB *, ListDB *);

