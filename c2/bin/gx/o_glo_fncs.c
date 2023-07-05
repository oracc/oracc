#include "iterator.h"

static void o_glo_aliases(struct entry *e);
static void o_glo_allow(struct entry *e);
static void o_glo_bases(struct entry *e);
static void o_glo_cbd(struct cbd *c);
static void o_glo_dcfs(struct entry *c);
static void o_glo_end_cbd(struct cbd *c);
static void o_glo_end_entry(struct entry *e);
static void o_glo_entry(struct entry *e);
static void o_glo_forms(struct entry *e);
static void o_glo_meta(struct entry *e);
static void o_glo_parts(struct entry *e);
static void o_glo_phon(struct entry *e);
static void o_glo_root(struct entry *e);
static void o_glo_senses(struct entry *e);
static void o_glo_stems(struct entry *e);

static struct ifnc_map imap[] = {
  { if_alias , (iterator_fnc)o_glo_aliases },
  { if_allow , (iterator_fnc)o_glo_allow },
  { if_bases , (iterator_fnc)o_glo_bases },
  { if_cbd , (iterator_fnc)o_glo_cbd },
  { if_dcfs , (iterator_fnc)o_glo_dcfs },
  { if_end_cbd , (iterator_fnc)o_glo_end_cbd },
  { if_end_entry , (iterator_fnc)o_glo_end_entry },
  { if_entry , (iterator_fnc)o_glo_entry },
  { if_forms , (iterator_fnc)o_glo_forms },
  { if_meta , (iterator_fnc)o_glo_meta },
  { if_parts , (iterator_fnc)o_glo_parts },
  { if_phon , (iterator_fnc)o_glo_phon },
  { if_root , (iterator_fnc)o_glo_root },
  { if_senses , (iterator_fnc)o_glo_senses },
  { if_stems , (iterator_fnc)o_glo_stems },
  { if_top , NULL },
};

static iterator_fnc *fncs;

static iterator_fnc *
ifnc_init(void)
{
  int i;
  fncs = calloc(if_top, sizeof(iterator_fnc));
  for (i = 0; i < if_top; ++i)
    fncs[imap[i].ix] = imap[i].fnc;
  return fncs;
}

