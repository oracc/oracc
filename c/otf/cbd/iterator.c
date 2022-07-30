#include "gx.h"
#include "iterator.h"

static iterator_fnc *curr_fncs;

void
iterate_entries(struct entry *e)
{
  curr_fncs[if_entry](e);
  if (list_len(e->aliases))
    list_exec(e->aliases, (list_exec_func*)curr_fncs[if_alias]);
  if (e->parts)
    curr_fncs[if_parts](e);
  curr_fncs[if_end_entry](e);
}

void
iterator(struct cbd *c, iterator_fnc *fncs)
{
  curr_fncs = fncs;
  curr_fncs[if_cbd](c);
  list_exec(c->entries, (list_exec_func*)iterate_entries);
  curr_fncs[if_end_cbd](c);
}
