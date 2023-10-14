%{
#include <string.h>
#include "sll.h"
%}
struct sllext;
%%
cmemb, ""," Compounds", SLL_SN, sll_get_cpd
cinit, "Initial ", "", SLL_SN, sll_get_cpd
clast, "Final ", "", SLL_SN, sll_get_cpd
contains, "", "Container", SLL_SN, sll_get_cpd
contained, "", "", SLL_SN, sll_get_cpd
forms, "", " Forms", SLL_ID, sll_get_forms
h, ""," Homophones", SLL_V, sll_get_h
name, "", "", SLL_ANY, sll_get_one_n
oid, "", "", SLL_ANY, sll_get_one_o
let,"","",SLL_ANY, sll_get_one_let
