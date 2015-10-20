/* weights.i */
%module weights
%{
#include "weights.h"

%}

extern double *weights_load_from_file(char *);
extern void weights_save_to_file(char *, uint, double *);

