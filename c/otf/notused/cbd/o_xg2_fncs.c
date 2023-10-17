#include "iterator.h"

static void o_xg2_aliases(struct entry *e);
static void o_xg2_allow(struct entry *e);
static void o_xg2_bases(struct entry *e);
static void o_xg2_cbd(struct cbd *c);
static void o_xg2_dcfs(struct entry *c);
static void o_xg2_end_cbd(struct cbd *c);
static void o_xg2_end_entry(struct entry *e);
static void o_xg2_entry(struct entry *e);
static void o_xg2_forms(struct entry *e);
static void o_xg2_meta(struct entry *e);
static void o_xg2_parts(struct entry *e);
static void o_xg2_phon(struct entry *e);
static void o_xg2_root(struct entry *e);
static void o_xg2_senses(struct entry *e);
static void o_xg2_stems(struct entry *e);

static struct ifnc_map imap[] = {
  { if_alias , (iterator_fnc)o_xg2_aliases },
  { if_allow , (iterator_fnc)o_xg2_allow },
  { if_bases , (iterator_fnc)o_xg2_bases },
  { if_cbd , (iterator_fnc)o_xg2_cbd },
  { if_dcfs , (iterator_fnc)o_xg2_dcfs },
  { if_end_cbd , (iterator_fnc)o_xg2_end_cbd },
  { if_end_entry , (iterator_fnc)o_xg2_end_entry },
  { if_entry , (iterator_fnc)o_xg2_entry },
  { if_forms , (iterator_fnc)o_xg2_forms },
  { if_meta , (iterator_fnc)o_xg2_meta },
  { if_parts , (iterator_fnc)o_xg2_parts },
  { if_phon , (iterator_fnc)o_xg2_phon },
  { if_root , (iterator_fnc)o_xg2_root },
  { if_senses , (iterator_fnc)o_xg2_senses },
  { if_stems , (iterator_fnc)o_xg2_stems },
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
