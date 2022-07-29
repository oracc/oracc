%{
#include "gx.h"
#define YY_NO_UNISTD_H
%}
%%
^\+?@entry		return(entry);
.|\n
%%
