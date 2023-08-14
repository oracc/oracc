#ifndef SIGNLIST_H_
#define SIGNLIST_H_

#include <hash.h>
#include <list.h>
#include <pool.h>
#include <tree.h>
#include <mesg.h>
#include <gsort.h>

struct sl_inst;

enum sx_tle { sx_tle_componly , sx_tle_formproxy , sx_tle_lref , sx_tle_sign , sx_tle_sref };

struct sl_signlist
{
  const char *project;
  struct sl_inst *notes;/* Allow inotes etc., after @signlist */
  Hash *listdefs; 	/* Hash of signlist names; value is struct sl_listdef */
  Hash *htoken; 	/* Every token that is a sign/form/list/value
			   as a struct sl_token * */
  Hash *hsentry; 	/* All the @sign/@sign- entries in the signlist */
  Hash *hfentry; 	/* All @form/@form- entries in signlist; host for sl_form* */
  Hash *hventry; 	/* All @v/@v- entries in signlist; host for sl_value* */
  Hash *hlentry; 	/* All @list/@list- entries in signlist; host for sl_list* */
  Hash *hsignvvalid; 	/* All @v which belong to a @sign,
			   not those belonging to @form; no @v- */
  Hash *homophones;	/* Hash of value-bases each with list of
			   sl_split_value* that reduce to that base;
			   x-values include the 'ₓ' in their base */
  Hash *hcompoundnew;	/* Signs reported in sx_compound_new_sign to prevent multiple error messages */
  Hash *values_by_oid;
  Hash *hletters;
  struct sl_token **tokens; /* sorted htoken */
  struct sl_sign  **signs;  /* sorted hsentry */
  int nsigns;
  struct sl_form  **forms;  /* sorted hfentry */
  int nforms;
  struct sl_value **values;  /* sorted hventry */
  int nvalues;
  struct sl_list  **lists;  /* sorted hlentry */
  int nlists;
  struct sl_letter *letters;
  int nletters;
  struct sl_split_value *splitv;
  struct sl_sign *curr_sign;
  struct sl_inst *curr_form;
  struct sl_inst *curr_value;
  struct sl_inst *curr_inst; /* used to attach meta to correct tag */
  List *compounds;
  Memo *m_tokens;
  Memo *m_letters;
  Memo *m_groups;
  Memo *m_signs;
  Memo *m_signs_p;
  Memo *m_forms;
  Memo *m_lists;
  Memo *m_values;
  Memo *m_insts;
  Memo *m_insts_p;
  Memo *m_lv_data;
  Memo *m_split_v;
  Memo *m_compounds;
  Memo *m_digests;
  Memo *m_parents;
  Pool *p;
  Mloc mloc;
};

struct sl_listdef
{
  unsigned const char *name;
  const char **names;
  int nnames;
  int sorted;
  Hash *seen;
  const char *str;
};

struct sl_token
{
  const unsigned char *t;	/* sign/form/value/list name token */
  Node *gdl;			/* token as GDL */
  GS_head *gsh;			/* GDL gsort data */
  int s;			/* sort code for token */
};

/* Parents of values */
struct sl_parents
{
  List *signs; /* list of insts; plural because xvalues can legally have multiple parents */
  List *forms; /* list of insts */
  /* Sorted array of OIDs for easy printing; if qvsign and qvform it's
     all OIDs; if just qvform it's only form OIDs */
  const char **qvoids;
};

struct sl_split_value
{
  const unsigned char *b;
  int i;
  const char *oid;
};

/* each of the lists in sl_any_note is a list of char*; handlers should
   be passed the owner sl_inst so an Mloc is available */
struct sl_any_note
{
  List *lit;
  List *notes;
  List *inotes;
};

struct sl_unicode
{
  const unsigned char *uchar;
  const char *ucode;
  const char *uphase;
  const char *uname;
  List *unotes;
};

/* List and value data for @form insts */
struct sl_lv_data
{
  Hash *hlentry; 	/* All @list entries */
  /*Hash *hlvalid;*/	/* @list entries except @list- ; NULL unless there is an @list- */
  Hash *hventry;	/* All @v entries */
  Hash *hvbases;	/* All @v bases; used only for checking duplicates like a₂ and a₃ within a @form */
  /*Hash *hvvalid;*/	/* @v entries except @v- ; NULL unless there is an @v- */
  Hash *hivalues; 	/* Inherited values from parent @sign's hventry */
  struct sl_inst **lists;
  int nlists;
  struct sl_inst **values;
  int nvalues;
  struct sl_inst **ivalues; /* inherited values */
  int nivalues;
};

struct sl_inst
{
  char type; /* S = signlist; s = sign; f = form; l = list; v = value */
  union {
    struct sl_signlist *S;
    struct sl_sign *s;
    struct sl_form *f;
    struct sl_list *l;
    struct sl_value *v; } u;
  struct sl_lv_data *lv; /* used by form instances */
  const unsigned char *ref; /* this is inline in the @v */
  const unsigned char *var; /* The variant code for a form instance, with tilde */
  struct sl_inst *parent_s; /* The parent sign for a form or value instance; if NULL use parent_f */
  struct sl_inst *parent_f; /* The parent form for a value instance */
  struct sl_any_note n;
  Mloc mloc;
  Boolean valid; /* doesn't have a - after it */
  Boolean inherited;
  Boolean literal;
  Boolean query;
  Boolean uchar;
  Boolean ucode;
  Boolean uname;
  Boolean uphase;
};

struct sl_letter
{
  const unsigned char *name;
  int code; 		/* integer of first character; used in
			   directory names, e.g., l0065, l0352, but
			   not suitable for sorting */
  Hash *hgroups;	/* hash of groups used while building structure */
  struct sl_group *groups;
  int ngroups;
};

struct sl_group 
{
  const unsigned char *name;
  Hash *hentry; 	/* Hash of sl_inst *that belong to this group */
  struct sl_inst **signs;
  int nsigns;
};

struct sl_compound
{
  char initial_or_final; /* 0 for no; -1 for initial; 1 for final */
  char medial; 		 /* 0 for no; 1 for singleton occurrence; 2
			    for multiple occurences. This means that
			    if a final instance of the sign is
			    encountered member can be decremented and
			    will zero out if the final member is also
			    the only medial member, but remain 1 if
			    the sign is, e.g., |U.U.U| */
  char container; 	 /* 0 for no 1 for yes */
  char contained; 	 /* 0 for no 1 for yes */
};

/* These are NULL-terminated arrays of OID or compound-sign-names
   which are derived from the sl_compound data */
struct sl_compound_digest
{
  const char **memb;
  const char **initial;
  const char **medial;
  const char **final;
  const char **container;
  const char **contained;
};

struct sl_sign
{
  struct sl_signlist *sl;
  const unsigned char *name;
  int name_is_listnum;
  Hash *hlentry; 	/* All @list entries */
  Hash *hventry;	/* All @v entries */
  Hash *hvbases;	/* All @v bases; used only for checking duplicates like a₂ and a₃ within a @sign */
  Hash *hfentry;	/* All @form entries */
  Hash *hcompounds;	/* Compound data: sign S has hash of names of
			   compounds C with hashvals consisting of
			   struct sl_compound; two special items are
			   hashed: '#digest_by_oid' and
			   '#digest_by_name'; the hashvals for these
			   are both sl_compound_digest */
  unsigned const char *letter;
  unsigned const char *group;
  struct sl_inst **lists;
  int nlists;
  struct sl_inst **values;
  int nvalues;
  struct sl_inst **forms;
  int nforms;
  struct sl_unicode U;
  const unsigned char *pname; /* For type=sign this is the plus-name
				 (e.g., |GA₂×A+HA| = |GA₂×(A.HA)|);
				 for other types it is the map value
				 from, e.g., @sref BA => BU */
  List *aka;		/* alternatively known as sign-names to support non-standard names */
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
  Boolean uchar;
  Boolean ucode;
  Boolean uname;
  Boolean uphase;
  Boolean fake;
  Boolean compound_only;
  enum sx_tle type;
#if 0
  Mloc *mloc; /* Or: keep this as indicator of "defining instance" ? */
#endif
};

struct sl_form
{
  const unsigned char *name;
  const unsigned char *pname;
  struct sl_sign *sign; /* This always points to a sign that
			   corresponds to the form; if the form
			   doesn't occur as an @sign entry, the
			   back-reference form->sign->xref is set */
  List *owners; 	/* this is a list of sl_sign* the form is associated with */
  struct sl_sign **owners_sort; /* owners as sorted array */
  int nowners;
  List *insts; 		/* this is a list of sl_inst* where the form occurs */
  List *aka;		/* alternatively known as form-names to support non-standard names */
  int name_is_listnum;
  int sort;
  const char *oid;
  struct sl_unicode U;
};

struct sl_list
{
  const unsigned char *name;
  const unsigned char *base;
  const unsigned char *num;
  int sort;
  List *insts; 		/* signs or forms where this list occurs */
  const char **oids; 	/* NULL-terminated, sorted and uniqued list of
			   sign/form names from the list's insts */
};

/* This is the global value information structure */
struct sl_value
{
  const unsigned char *name;
  const unsigned char *base; /* without index, e.g., for a₃ this is 'a' */
  const char *lang; 	  /* this is inline in the @v; it's an error
			     for two @v to have different lang */
  struct sl_sign *sowner; /* for a value at the sign level, this is
			     the sign it belongs to; may be NULL if
			     value only occurs in forms */
  List *fowners; 	  /* for a value at the form level, this is a
			     list of sl_inst* it belongs to */
  List *insts;
  struct sl_parents *parents; /* Just the valid parents computed from the insts */
  struct sl_inst **fowners_i_sort; /* The fowners as an array of
				      sorted pointers to sl_insts */
  int nfowners;
  int sort;
  const char **oids; 	  /* NULL-terminated, sorted and uniqued list of
			     sign/form names from the value's sign and form
			     insts */
  Boolean atf;		  /* value is not a simple grapheme but an atf transliteration, e.g., u-gun₃ */
  Boolean xvalue;      	  /* value ends in ₓ */
  Boolean qvsign;	  /* value must be qualified when it has a sign parent */
  Boolean qvform;	  /* value must be qualified when it has a form parent */
  Boolean qvmust;	  /* Value has no unqualified version */
  Boolean unknown;     	  /* name is 'x'; these are SIGN entries in
			     lex whose value is not preserved */
  unsigned char index; 	  /* 1 for no index; integer value of index for numeric indices; 255 for sub x */
};

extern struct sl_signlist *asl_bld_init(void);
extern void asl_bld_listdef(Mloc *locp, struct sl_signlist *sl, const char *name, const char *in);
extern void asl_bld_form(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
			 int list, const unsigned char *ref, int minus_flag);
extern void asl_bld_list(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int minus_flag);
extern void asl_bld_aka(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_pname(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_comp(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int list);

extern void asl_bld_tle(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, const unsigned char *m, enum sx_tle type);
extern void asl_bld_sign(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
			 int list, int minus_flag);
extern void asl_bld_signlist(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
					    int list);
extern void asl_bld_term(struct sl_signlist *);
extern void asl_bld_token(Mloc *locp, struct sl_signlist *sl, unsigned char *t, int literal);

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

extern void asl_bld_end_sign(Mloc *locp, struct sl_signlist *sl);

#endif/*SIGNLIST_H_*/
