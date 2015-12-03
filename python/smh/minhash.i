/* minhash.i */
%module minhash
%include typemaps.i
%{
#include "listdb.h"
#include "minhash.h"

%}

extern void mh_rng_init(unsigned long long);
extern uint * mh_get_cumulative_frequency(ListDB *, ListDB *);
extern ListDB mh_expand_listdb(ListDB *, uint *);
extern double * mh_expand_weights(uint, uint *, double *);
