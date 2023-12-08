#include <stdlib.h>
#include "colltype.h"

extern struct Collate_info unicode_collate_info;

struct Collate_info_list_node collate_infos[] = 
{
  { "unicode", &unicode_collate_info },
  { NULL, NULL },
};
