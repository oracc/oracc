%{
#include <stdlib.h>
#include <string.h>
#include <cat.h>
#include "ax.h"
%}
struct catinfo;
%%
xtf,    		CI_PARENT , 1 , 0, ax_parse,  NULL, NULL
amp,    		CI_PARENT , 2 , 0, ax_parse,  NULL, NULL
translation,    CI_PARENT , 3 , 0, ax_parse,  NULL, NULL
line,    		CI_CHILD , 4 , 0, ax_gparse, NULL, NULL
siglum,    		CI_CHILD , 4 , 0, ax_gparse, NULL, NULL
"#default", 	CI_CHILD  , 4 , 0, ax_parse,  NULL, NULL
