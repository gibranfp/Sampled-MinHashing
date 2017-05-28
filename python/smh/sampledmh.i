/* sampledmh.i */
%module sampledmh
%{
#include "listdb.h"
#include "sampledmh.h"

#define DEF_TABLE_SIZE 524288
%}

extern ListDB sampledmh_mine(ListDB *, uint, uint, uint, uint);
extern ListDB sampledmh_mine_weighted(ListDB *, uint, uint, uint,
                                      double *, uint);
extern void sampledmh_prune(ListDB *, ListDB *, uint, uint, double, double);

