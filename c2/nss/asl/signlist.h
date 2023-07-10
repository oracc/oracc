#ifndef SIGNLIST_H_
#define SIGNLIST_H_

#include <hash.h>
#include <pool.h>
#include <tree.h>

struct sl_signlist
{
  Hash *signs;
  Hash *forms_which_are_not_signs;
  struct sl_sign *sorted_signs;
  struct letter *letters;
  struct sign *curr_sign;
  struct form *curr_form;
  struct value *curr_value;
  Memo *m_letters;
  Memo *m_groups;
  Memo *m_signs;
  Memo *m_forms;
  Memo *m_lists;
  Memo *m_values;
  Pool *p;
};

struct sl_letter
{
  const unsigned char *l;
  struct sl_group *groups;
};

struct sl_group 
{
  const unsigned char *sign;
  struct sl_sign *signs;
};

struct sl_sign
{
  const unsigned char *name;
  const char *uphase;
  const char *uname;
  const unsigned char *utf8;
  const char *oid;
  List *lit;
  List *notes;
  List *inotes
  List *unotes;
  int nosign;
  int fake;
  int query;
  int name_is_listnum;
  Tree *gdl;
  Hash *lists;
  Hash *forms;
  Hash *values;
  struct sl_value *sorted_values;
  int sort;
  struct sl_form *xref; /* this sign is a header for the @form which
			   defines the sign name; sort value is sort
			   sequence with signs */
};

struct sl_form
{
  const unsigned char *name;
  const char *uphase;
  const char *uname;
  const unsigned char *utf8;
  const char *oid;
  List *lit;
  List *notes;
  List *inotes
  List *unotes;
  int noform;
  int query;
  Tree *gdl;
  Hash *lists;
  Hash *values;
  Hash *inherited_values;
  struct sl_value *sorted_values;
};

struct sl_list
{
  const unsigned char *name;
  const unsigned char *base;
  const unsigned char *num;
  int query;
};

struct sl_value
{
  const unsigned char *name;
  const char *lang;
  const unsigned char *comment;
  const unsigned char *bib;
  const char *uphase;
  const char *uname;
  const unsigned char *utf8;
  List *lit;
  List *notes;
  List *inotes
  List *unotes;
  int novalue;
  int deprecated;
  int query;
};

#endif/*SIGNLIST_H_*/
