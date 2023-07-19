%{
#include <stdlib.h>
#include <string.h>
#include <cat.h>
#include "sx.h"
%}
struct catinfo;
%%
signs,  CI_PARENT , 1 , 0, NULL, 	  NULL, NULL
sign,   CI_PARENT , 2 , 1, sx_gparse, NULL, NULL
form, 	CI_PARENT , 3 , 0, sx_gparse, NULL, NULL
v,		CI_CHILD  , 0 , 0, sx_gparse, NULL, NULL
end,	CI_END 	  , 0 , 0, NULL, 	  NULL, NULL
