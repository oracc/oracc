%{
#include <string.h>
#include "translate.h"
#include "xmlnames.h"
%}
struct trans_token
{
    const char*     name;
    enum e_tu_types type;
    enum e_type     elem;
}
%%
parallel, etu_parallel , e_enum_top
labeled,  etu_labeled  , e_enum_top
unitary,  etu_unitary  , e_enum_top
