#ifndef SIGNLIST_H_
#define SIGNLIST_H_

#include <hash.h>
#include <list.h>
#include <pool.h>
#include <tree.h>
#include <mesg.h>

struct sl_signlist
{
  const char *project;
  Hash *hsigns; 	/* contains signs and is augmented with forms
			   which are not also signs */
  Hash *hforms; 	/* constains all forms */
  Hash *hvalues; 	/* contains all values */
  Hash *hlists; 	/* contains all sl_list* */
  Hash *hletters;
  Hash *hsignvalues; 	/* contains only values which belong to @sign,
			   not those belonging to @form */
  struct sl_sign **signs;
  int nsigns;
  struct sl_form **forms;
  int nforms;
  struct sl_list **lists;
  int nlists;
  struct sl_value **values;
  int nvalues;
  struct sl_letter *letters;
  int nletters;
  struct sl_sign *curr_sign;
  struct sl_inst *curr_form;
  struct sl_inst *curr_value;
  Memo *m_letters;
  Memo *m_groups;
  Memo *m_signs;
  Memo *m_forms;
  Memo *m_lists;
  Memo *m_values;
  Memo *m_insts;
  Memo *m_signs_p;
  Memo *m_lv_data;
  Pool *p;
  Mloc *mloc;
};

/* each of the lists in sl_any_note is a list of sl_str_loc* */
struct sl_str_loc
{
  const unsigned char *s;
  Mloc *mloc;
};

struct sl_any_note
{
  List *lit;
  List *notes;
  List *inotes;
  List *comments;
};

struct sl_unicode_info
{
  const unsigned char *uchar;
  const char *ucode;
  const char *uphase;
  const char *uname;
  List *unotes;
};

/* List and value data for @sign and @form insts */
struct sl_lv_data
{
  Hash *hlists;
  Hash *hvalues;
  Hash *hivalues;
  struct sl_inst **lists;
  int nlists;
  struct sl_inst **values;
  int nvalues;
  struct sl_inst **ivalues; /* inherited values */
  int nivalues;
};

struct sl_inst
{
  char type;
  union {
    struct sl_sign *s;
    struct sl_form *f;
    struct sl_list *l;
    struct sl_value *v; } u;
  struct sl_lv_data *lv; /* used by form instances */
  const unsigned char *ref; /* this is inline in the @v */
  struct sl_any_note n;
  Boolean query;
  Boolean removed;
  Mloc *mloc;
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
  int nsigns;
};

struct sl_sign
{
  const unsigned char *name;
  int name_is_listnum;
  Node *gdl;
  unsigned const char *letter;
  unsigned const char *group;
  Hash *hlists;
  Hash *hvalues;
  Hash *hforms;
  struct sl_inst **lists;
  int nlists;
  struct sl_inst **values;
  int nvalues;
  struct sl_inst **forms;
  int nforms;
  struct sl_unicode_info U;
  int fake;
  int sort;
  const char *oid;
  struct sl_inst *inst;
  struct sl_form *xref; /* this sign is a header for the @form which
			   defines the sign name; sort value is in
			   sort sequence with signs; note that the
			   form might occur in multiple signs (it will
			   only get added to the global signs hash
			   once) so printed xrefs should use
			   sign->xref->form->owners which should also
			   be sorted before output */
  Mloc *mloc;
};

struct sl_form
{
  const unsigned char *name;
  const unsigned char *var; /* The variant code for the form, with tilde */
  Node *gdl;
  List *owners; /* this is a list of sl_sign* the form is associated with */
  List *insts; 	/* this is a list of sl_inst* where the form occurs */
  int name_is_listnum;
  int sort;
  struct sl_unicode_info U;
};

struct sl_list
{
  const unsigned char *name;
  const unsigned char *base;
  const unsigned char *num;
  int sort;
  List *insts; /* signs or forms where this list occurs */
};

/* This is the global value information structure */
struct sl_value
{
  const unsigned char *name;
  Node *gdl;
  const char *lang; 	  /* this is inline in the @v; it's an error
			     for two @v to have different lang */
  struct sl_sign *sowner; /* for a value at the sign level, this is
			     the sign it belongs to; may be NULL if
			     value only occurs in forms */
  List *fowners; 	  /* for a value at the form level, this is a
			     list of sl_inst* it belongs to */
  List *insts;
  int atf;
  int phonetic;
  int unknown; 		  /* name is 'x'; these are SIGN entries in
			     lex whose value is not preserved */
  int sort;
};

struct sl_functions;

typedef void (sl_signlist_f)(struct sl_functions *,struct sl_signlist*);
typedef void (sl_letter_f)(struct sl_functions *,struct sl_letter*);
typedef void (sl_group_f)(struct sl_functions *,struct sl_group*);
typedef void (sl_sign_f)(struct sl_functions *,struct sl_sign*);
typedef void (sl_list_f)(struct sl_functions *,struct sl_inst*);
typedef void (sl_form_f)(struct sl_functions *,struct sl_inst*);
typedef void (sl_value_f)(struct sl_functions *,struct sl_inst*);

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
extern void asl_bld_form(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
			 int list, const unsigned char *var, const unsigned char *ref, int minus_flag);
extern void asl_bld_list(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int minus_flag);
extern void asl_bld_sign(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
			 int list, int minus_flag);
extern struct sl_signlist *asl_bld_signlist(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
					    int list);
extern void asl_bld_term(struct sl_signlist *);
extern void asl_bld_ucode(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_uphase(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_unote(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_uname(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_uchar(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_value(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
			  const char *lang, const unsigned char *ref, int atf_flag, int minus_flag);
extern void asl_register_sign(Mloc *locp, struct sl_signlist *sl, struct sl_sign *s);

extern void asl_bld_inote(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_lit(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_note(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);

#endif/*SIGNLIST_H_*/
