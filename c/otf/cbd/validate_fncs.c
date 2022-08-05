#include "iterator.h"

static void v_alias(struct alias *a);
static void v_allow(struct entry *e);
static void v_bases(struct entry *e);
static void v_cbd(struct cbd *c);
static void v_dcfs(struct entry *c);
static void v_end_cbd(struct cbd *c);
static void v_end_entry(struct entry *e);
static void v_entry(struct entry *e);
static void v_forms(struct entry *e);
static void v_meta(struct entry *e);
static void v_parts(struct entry *e);
static void v_phon(struct entry *e);
static void v_root(struct entry *e);
static void v_senses(struct entry *e);
static void v_stems(struct entry *e);

static struct ifnc_map imap[] = {
  { if_alias , (iterator_fnc)v_alias },
  { if_allow , (iterator_fnc)v_allow },
  { if_bases , (iterator_fnc)v_bases },
  { if_cbd , (iterator_fnc)v_cbd },
  { if_dcfs , (iterator_fnc)v_dcfs },
  { if_end_cbd , (iterator_fnc)v_end_cbd },
  { if_end_entry , (iterator_fnc)v_end_entry },
  { if_entry , (iterator_fnc)v_entry },
  { if_forms , (iterator_fnc)v_forms },
  { if_meta , (iterator_fnc)v_meta },
  { if_parts , (iterator_fnc)v_parts },
  { if_phon , (iterator_fnc)v_phon },
  { if_root , (iterator_fnc)v_root },
  { if_senses , (iterator_fnc)v_senses },
  { if_stems , (iterator_fnc)v_stems },
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
