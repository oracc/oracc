#include "iterator.h"

static void i_aliases(struct entry *e);
static void i_allow(struct entry *e);
static void i_bases(struct entry *e);
static void i_cbd(struct cbd *c);
static void i_dcfs(struct entry *c);
static void i_end_cbd(struct cbd *c);
static void i_end_entry(struct entry *e);
static void i_entry(struct entry *e);
static void i_forms(struct entry *e);
static void i_meta(struct entry *e);
static void i_parts(struct entry *e);
static void i_phon(struct entry *e);
static void i_root(struct entry *e);
static void i_senses(struct entry *e);
static void i_stems(struct entry *e);

static struct ifnc_map imap[] = {
  { if_alias , (iterator_fnc)i_aliases },
  { if_allow , (iterator_fnc)i_allow },
  { if_bases , (iterator_fnc)i_bases },
  { if_cbd , (iterator_fnc)i_cbd },
  { if_dcfs , (iterator_fnc)i_dcfs },
  { if_end_cbd , (iterator_fnc)i_end_cbd },
  { if_end_entry , (iterator_fnc)i_end_entry },
  { if_entry , (iterator_fnc)i_entry },
  { if_forms , (iterator_fnc)i_forms },
  { if_meta , (iterator_fnc)i_meta },
  { if_parts , (iterator_fnc)i_parts },
  { if_phon , (iterator_fnc)i_phon },
  { if_root , (iterator_fnc)i_root },
  { if_senses , (iterator_fnc)i_senses },
  { if_stems , (iterator_fnc)i_stems },
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

