%{
#include <string.h>
#include "sll.h"
%}
struct sllext;
%%
aka, "", " Aliases", SN, sll_get_list
c, ""," Compounds", SN, sll_get_list
cinit, "Initial ", "", SN, sll_get_list
clast, "Final ", "", SN, sll_get_list
contains, "", "Container", SN, sll_get_list
contained, "",, SN, sll_get_list
forms, "", " Forms", ID, sll_get_list
h, ""," Homophones", SN, sll_get_h
m, "Modified ","", SN, sll_get_list
multi, "", " Repeated", SN, sll_get_list
signlist, "", " Numbers", SN, sll_get_list
