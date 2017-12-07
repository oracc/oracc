%{
#include <string.h>
#include "se.h"
#include "types.h"
#include "property.h"
%}
struct prop_tab;
%%
PN, PROP_PN, pg_properties
DN, PROP_DN, pg_properties
RN, PROP_RN, pg_properties
FN, PROP_FN, pg_properties
TN, PROP_TN, pg_properties
GN, PROP_GN, pg_properties
XN, PROP_XN, pg_properties
NM, PROP_NM, pg_properties
VM, PROP_VM, pg_properties
NL, PROP_NL, pg_properties
VL, PROP_VL, pg_properties
OL, PROP_OL, pg_properties
wb, PROP_WB, pg_properties
we, PROP_WE, pg_properties
lb, PROP_LB, pg_properties
le, PROP_LE, pg_properties
g, IF_GUIDE, pg_start_column
l, IF_LEMMA, pg_start_column
m1, IF_MORPH, pg_start_column
m2, IF_MORPH2, pg_start_column
