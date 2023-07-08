%{
#include <string.h>
#include "sll.h"
%}
struct sllext;
%%
aka, "", " Aliases", SLL_SN, sll_get_list
c, ""," Compounds", SLL_SN, sll_get_list
cinit, "Initial ", "", SLL_SN, sll_get_list
clast, "Final ", "", SLL_SN, sll_get_list
contains, "", "Container", SLL_SN, sll_get_list
contained, "", "", SLL_SN, sll_get_list
forms, "", " Forms", SLL_ID, sll_get_list
h, ""," Homophones", SLL_SN, sll_get_h
m, "Modified ","", SLL_SN, sll_get_list
multi, "", " Repeated", SLL_SN, sll_get_list
signlist, "", " Numbers", SLL_SN, sll_get_list
name, "", "", SLL_ANY, sll_get_one_n
oid, "", "", SLL_ANY, sll_get_one_o
