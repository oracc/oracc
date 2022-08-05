#include "gx.h"
#include "iterator.h"

static iterator_fnc *curr_fncs;

void
iterate_entries(struct entry *e)
{
  curr_fncs[if_entry](e);
  if (list_len(e->aliases))
    list_exec(e->aliases, (list_exec_func*)curr_fncs[if_alias]);
  if (list_len(e->dcfs))
    curr_fncs[if_dcfs](e);
  if (e->parts)
    curr_fncs[if_parts](e);
  if (e->allows && list_len(e->allows))
    curr_fncs[if_allow](e);
  if (e->phon)
    curr_fncs[if_phon](e);
  if (e->root)
    curr_fncs[if_root](e);
  if (e->stems)
    curr_fncs[if_stems](e);
  if (e->bases)
    curr_fncs[if_bases](e);
  if (e->forms)
    curr_fncs[if_forms](e);
  if (e->senses)
    curr_fncs[if_senses](e);
  if (e->meta)
    curr_fncs[if_meta](e);
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
