%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iome.h"
%}
struct iome;
%%
tg1, iome_tg1, "http://oracc.org/ns/cbd/2.0/g1", "cbd 1.0 text .glo file", NULL, NULL
tg2, iome_tg2, "http://oracc.org/ns/cbd/2.0/g2", "cbd 2.0 text .glo file", NULL, NULL
xc1, iome_xc1, "http://oracc.org/ns/cbd/2.0/g1", "cbd 1.0 .xml form of .glo", NULL, NULL
xc2, iome_xc2, "http://oracc.org/ns/cbd/2.0/g2", "cbd 2.0 .xml form of .glo", NULL, NULL
x11, iome_x11, "http://oracc.org/ns/cbd/2.0/c1", "cbd 1.0 .cbd file (xml phase1)", NULL, NULL
x12, iome_x12, "http://oracc.org/ns/cbd/2.0/c2", "cbd 2.0 .cbd file (xml phase1)", NULL, NULL
x21, iome_x21, "http://oracc.org/ns/cbd/2.0/x1", "cbd 1.0 .g2x file (xml phase2)", NULL, NULL
x22, iome_x22, "http://oracc.org/ns/cbd/2.0/x2", "cbd 2.0 .g2x file (xml phase2)", NULL, NULL
