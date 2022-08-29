#include "iterator.h"

static void xo_aliases(struct entry *e);
static void xo_allow(struct entry *e);
static void xo_bases(struct entry *e);
static void xo_cbd(struct cbd *c);
static void xo_dcfs(struct entry *c);
static void xo_end_cbd(struct cbd *c);
static void xo_end_entry(struct entry *e);
static void xo_entry(struct entry *e);
static void xo_forms(struct entry *e);
static void xo_meta(struct entry *e);
static void xo_parts(struct entry *e);
static void xo_phon(struct entry *e);
static void xo_root(struct entry *e);
static void xo_senses(struct entry *e);
static void xo_stems(struct entry *e);

static struct ifnc_map imap[] = {
  { if_alias , (iterator_fnc)xo_aliases },
  { if_allow , (iterator_fnc)xo_allow },
  { if_bases , (iterator_fnc)xo_bases },
  { if_cbd , (iterator_fnc)xo_cbd },
  { if_dcfs , (iterator_fnc)xo_dcfs },
  { if_end_cbd , (iterator_fnc)xo_end_cbd },
  { if_end_entry , (iterator_fnc)xo_end_entry },
  { if_entry , (iterator_fnc)xo_entry },
  { if_forms , (iterator_fnc)xo_forms },
  { if_meta , (iterator_fnc)xo_meta },
  { if_parts , (iterator_fnc)xo_parts },
  { if_phon , (iterator_fnc)xo_phon },
  { if_root , (iterator_fnc)xo_root },
  { if_senses , (iterator_fnc)xo_senses },
  { if_stems , (iterator_fnc)xo_stems },
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
