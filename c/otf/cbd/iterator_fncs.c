#include "iterator.h"

static void f_alias(struct alias *a);
static void f_allow(struct entry *e);
static void f_bases(struct entry *e);
static void f_cbd(struct cbd *c);
static void f_end_cbd(struct cbd *c);
static void f_end_entry(struct entry *e);
static void f_entry(struct entry *e);
static void f_forms(struct entry *e);
static void f_meta(struct entry *e);
static void f_parts(struct entry *e);
static void f_phon(struct entry *e);
static void f_root(struct entry *e);
static void f_senses(struct entry *e);
static void f_stems(struct entry *e);

struct ifnc_map {
  enum ifnc ix;
  iterator_fnc fnc;
} imap[] = {
  { if_alias , (iterator_fnc)f_alias },
  { if_allow , (iterator_fnc)f_allow },
  { if_bases , (iterator_fnc)f_bases },
  { if_cbd , (iterator_fnc)f_cbd },
  { if_end_cbd , (iterator_fnc)f_end_cbd },
  { if_end_entry , (iterator_fnc)f_end_entry },
  { if_entry , (iterator_fnc)f_entry },
  { if_forms , (iterator_fnc)f_forms },
  { if_meta , (iterator_fnc)f_meta },
  { if_parts , (iterator_fnc)f_parts },
  { if_phon , (iterator_fnc)f_phon },
  { if_root , (iterator_fnc)f_root },
  { if_senses , (iterator_fnc)f_senses },
  { if_stems , (iterator_fnc)f_stems },
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

