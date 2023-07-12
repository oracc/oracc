#ifndef SIGNLIST_H_
#define SIGNLIST_H_

#include <hash.h>
#include <list.h>
#include <pool.h>
#include <tree.h>

/*enum sl_type_codes { SL_SIGNLIST , SL_LETTER , SL_GROUP , SL_SIGN , SL_FORM , SL_LIST, SL_VALUE, SL_MAX_TYPE };*/

struct sl_signlist
{
  const char *project;
  Hash *signs;
  Hash *forms_which_are_not_signs;
  Hash *signvalues;
  Hash *hletters;
  struct sl_sign *signs;
  struct sl_letter *letters;
  int nletters;
  struct sl_sign *curr_sign;
  struct sl_form *curr_form;
  struct sl_value *curr_value;
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
  const unsigned char *name;
  struct sl_group *groups;
  int ngroups;
};

struct sl_group 
{
  const unsigned char *name;
  struct sl_sign **signs;
};

struct sl_signform_meta
{
  const char *oid;
  const char *uphase;
  const char *uname;
  const unsigned char *utf8;
  List *unotes;
};

struct sl_any_note
{
  List *lit;
  List *notes;
  List *inotes;
};

struct sl_sign
{
  const unsigned char *name;
  Node *gdl;
  unsigned const char *letter;
  unsigned const char *group;
  Hash *hlists;
  Hash *hvalues;
  Hash *hforms;
  struct sl_list *lists;
  struct sl_value *values;
  struct sl_form *forms;
  struct sl_signform_meta m;
  struct sl_any_note n;
  int nosign;
  int fake;
  int query;
  int name_is_listnum;
  int sort;
  struct sl_form *xref; /* this sign is a header for the @form which
			   defines the sign name; sort value is sort
			   sequence with signs */
};

struct sl_form
{
  const unsigned char *name; /* The variant code for the form, with tilde */
  const unsigned char *sign; /* The sign name for the form */
  Node *gdl;
  unsigned const char *letter;
  unsigned const char *group;
  Hash *hlists;
  Hash *hivalues;
  Hash *hvalues;
  struct sl_value *values;
  struct sl_value *ivalues; /* inherited values */
  struct sl_signform_meta m;
  struct sl_any_note n;
  int noform;
  int query;
};

struct sl_list
{
  const unsigned char *name;
  const unsigned char *base;
  const unsigned char *num;
  struct sl_any_note n;
  int query;
};

struct sl_value
{
  const unsigned char *name;
  Node *gdl;
  const char *lang; /* this is inline in the @v */
  const unsigned char *comment; /* this is inline in the @v */
  const unsigned char *bib; /* this is inline in the @v */
  struct sl_any_note n;
  int novalue;
  int deprecated;
  int query;
};


struct sl_functions;

typedef void (sl_signlist_f)(struct sl_functions *,struct sl_signlist*);
typedef void (sl_letter_f)(struct sl_functions *,struct sl_letter*);
typedef void (sl_group_f)(struct sl_functions *,struct sl_group*);
typedef void (sl_sign_f)(struct sl_functions *,struct sl_sign*);
typedef void (sl_list_f)(struct sl_functions *,struct sl_list*);
typedef void (sl_form_f)(struct sl_functions *,struct sl_form*);
typedef void (sl_value_f)(struct sl_functions *,struct sl_value*);

#include <stdio.h>

struct sl_functions
  {
    sl_signlist_f *sll;
    sl_letter_f *let;
    sl_group_f *grp;
    sl_sign_f *sgn;
    sl_form_f *frm;
    sl_list_f *lst;
    sl_value_f *val;
    FILE *fp;
    const char *fname;
  };

extern struct sl_signlist *asl_bld_init(void);
extern void asl_bld_term(struct sl_signlist *);
extern void asl_bld_sign(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int list);

#endif/*SIGNLIST_H_*/
