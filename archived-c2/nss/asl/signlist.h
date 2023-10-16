#ifndef SIGNLIST_H_
#define SIGNLIST_H_

#include <hash.h>
#include <list.h>
#include <roco.h>
#include <pool.h>
#include <tree.h>
#include <mesg.h>
#include <gsort.h>

struct sl_inst;
struct sl_listdef;

enum sx_tle
  {
    sx_tle_none ,
    sx_tle_componly ,
    sx_tle_fcomponly ,
    sx_tle_formproxy ,
    sx_tle_lref ,
    sx_tle_sign ,
    sx_tle_sref
  };

struct sx_iheader
{
  Roco *r;
  int order;
  const char *id;
  const char *label;
  const char *path;
  const char *period;
};

struct sl_signlist
{
  const char *project;
  struct sl_inst *notes;/* Allow inotes etc., after @signlist */
  Hash *listdefs; 	/* Hash of signlist names; value is struct sl_listdef */
  Hash *sysdefs; 	/* Hash of system names; value is struct sl_sysdef */
  Hash *htoken; 	/* Every token that is a sign/form/list/value
			   as a struct sl_token * */
  Hash *hsentry; 	/* All the @sign/@sign- entries in the signlist */
  Hash *hfentry; 	/* All @form/@form- entries in signlist; host for sl_form* */
  Hash *haka;		/* Hash of @aka entries pointing to sl_sign or sl_form */
  Hash *hventry; 	/* All @v/@v- entries in signlist; host for sl_value* */
  Hash *hlentry; 	/* All @list/@list- entries in signlist; host for sl_list* */
  Hash *hsignvvalid; 	/* All @v which belong to a @sign,
			   not those belonging to @form; no @v- */
  Hash *homophones;	/* Hash of value-bases each with list of
			   sl_split_value* that reduce to that base;
			   x-values include the 'ₓ' in their base */
  Hash *hcompoundnew;	/* Signs reported in sx_compound_new_sign to prevent multiple error messages */
  Hash *values_by_oid;
  Hash *oidindexes;
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
  List *syslists; 		/* list of the lists of @sys that occur in sign or
		     		   form, so we can generate system tables easily */
  List *images; 		/* list of names of image manifests as char * */
  struct sx_iheader *iheaders;	/* array of header data read from @cmds in image manifests */
  Roco *iarray;			/* images data read into a Roco array */  
  Hash *oid2ucode;
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
  Memo *m_notes;
  Memo *m_memostr;
  Memo *m_syss;
  Memo *m_links;
  Pool *p;
  Mloc mloc;
};

struct sl_token
{
  const unsigned char *t;	/* sign/form/value/list name token */
  Node *gdl;			/* token as GDL */
  GS_head *gsh;			/* GDL gsort data */
  const char *gsig;		/* return value from gdlsig run on
				   parsed GDL; NULL if the token was a
				   literal l*/
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

/* Note information is stored in a single list so that within a note
   group the order is preserved in identity output; note groups may
   move within a sign block because they are only attached to items
   that are pointed to by sl->curr_inst */
struct sl_note
{
  const char *tag;
  const char *txt;
};

struct sl_unicode
{
  const unsigned char *utf8; 	/* the character(s) in UTF-8 */
  const char *uhex;		/* the U+HHHHH code for an encoded character */
  const char *useq;		/* for characters not encoded as singletons, a sequence of hex values to render the sign name */
  const char *upua; 		/* for unencoded character(s), the hex code for a PUA codepoint in the form xXXXXX */
  const char *umap; 		/* map of current @sign or @form to another @sign or @form for obtaining Unicode info */
  const char *urev; 		/* the Unicode revision */
  const char *uname;		/* the Unicode name */
  List *unotes;			/* Unicode-related notes on the character and possibly related characters */
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
  char type; /* S = signlist; d = listdef; y = sysdef; s = sign; f = form; l = list; v = value */
  union {
    struct sl_signlist *S;
    struct sl_listdef *d;
    struct sl_sysdef *y;
    struct sl_sign *s;
    struct sl_form *f;
    struct sl_list *l;
    struct sl_value *v; } u;
  struct sl_lv_data *lv; 	/* used by form instances */
  struct sl_inst *parent_s; 	/* The parent sign for a form or value instance; if NULL use parent_f */
  struct sl_inst *parent_f; 	/* The parent form for a value instance */
  List *notes;			/* A list of struct sl_note * */
  List *sys;			/* A list of @sys in a sign or form */
  const char *lang; 	  	/* this is inline in the @v; an x-value could have a lang with one sign but not another */
  Mloc mloc;
  Boolean valid; /* doesn't have a - after it */
  Boolean inherited;
  Boolean literal;
  Boolean query;
  Boolean upua;
  Boolean utf8;
  Boolean uhex;
  Boolean umap;
  Boolean useq;
  Boolean uname;
  Boolean urev;
};

struct sl_listdef
{
  unsigned const char *name;
  const char **names;
  int nnames;
  int sorted;
  Hash *known;
  Hash *seen;
  const char *str;
  struct sl_inst inst;
};

struct sl_sysdef
{
  unsigned const char *name;
  const char *comment;
  struct sl_inst inst;
};

struct sl_sys
{
  const char *name;
  unsigned const char *v;
  unsigned const char *vv;
};

struct sl_letter
{
  const unsigned char *name;
  int code; 		/* integer of first character; used in
			   directory names, e.g., l0065, l0352, but
			   not suitable for sorting */
  const char *lname;	/* letter name set for number_group */
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
  const unsigned char *pname; 	/* For type=sign this is the plus-name
				   (e.g., |GA₂×A+HA| = |GA₂×(A.HA)|);
				   for other types it is the map value
				   from, e.g., @sref BA => BU */
  List *aka;			/* alternatively known as sign-names
				   to support non-standard names; data
				   type is Memo_str with literal flag
				   stored in user ptr */
  int sort;
  const char *oid;
  const char *smap;	   	/* Must be a @form within this sign */
  const char *smoid;	   	/* OID for @form in @smap */
  struct sl_inst *inst;
  struct sl_form *xref;        	/* this sign is a header for the @form
				   which defines the sign name; sort
				   value is in sort sequence with
				   signs; note that the form might
				   occur in multiple signs (it will
				   only get added to the global signs
				   hash once) so printed xrefs should
				   use sign->xref->form->owners which
				   should also be sorted before
				   output */
  Boolean fake;
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
  List *aka;		/* alternatively known as form-names to support non-standard names, data type is Memo_str */
  List *sys; 		/* List of @sys entries in sign */
  int name_is_listnum;
  int sort;
  const char *oid;
  struct sl_unicode U;
  Boolean compoundonly;
};

enum sl_ll_type { sl_ll_none , sl_ll_list , sl_ll_lref };
struct sl_list
{
  const unsigned char *name;
  const unsigned char *base;
  const unsigned char *num;
  int sort;
  List *insts; 		/* signs or forms where this list occurs */
  const char **oids; 	/* NULL-terminated, sorted and uniqued list of
			   sign/form names from the list's insts */
  struct sl_inst *inst;
  enum sl_ll_type type; /* sl_ll_list or sl_ll_lref */
};

/* This is the global value information structure */
struct sl_value
{
  const unsigned char *name;
  const unsigned char *base; /* without index, e.g., for a₃ this is 'a' */
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
extern void asl_bld_sysdef(Mloc *locp, struct sl_signlist *sl, const char *name, const char *comment);
extern void asl_bld_images(Mloc *locp, struct sl_signlist *sl, const unsigned char *n);
extern void asl_bld_form(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,int minus_flag);
extern void asl_bld_list(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int minus_flag);
extern void asl_bld_aka(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_smap(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_pname(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_comp(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int list);

extern void asl_bld_tle(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, const unsigned char *m, enum sx_tle type);
extern void asl_bld_sign(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,int minus_flag);
extern void asl_bld_signlist(Mloc *locp, const unsigned char *n, int list);
extern void asl_bld_sys(Mloc *locp, struct sl_signlist *sl, const char *sysname, unsigned const char *v, unsigned const char *vv);
extern void asl_bld_term(struct sl_signlist *);
extern void asl_bld_token(Mloc *locp, struct sl_signlist *sl, unsigned char *t, int literal);

extern void asl_bld_uhex(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_urev(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_useq(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_upua(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_utf8(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_umap(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_uname(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_unote(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_value(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
			  const char *lang, int atf_flag, int minus_flag);
extern void asl_register_sign(Mloc *locp, struct sl_signlist *sl, struct sl_sign *s);

extern void asl_bld_note(Mloc *locp, struct sl_signlist *sl, const char *tag, const char *txt);

extern void asl_bld_end_sign(Mloc *locp, struct sl_signlist *sl);
extern struct sl_sign *asl_form_as_sign(struct sl_signlist *sl, struct sl_form *f);

#endif/*SIGNLIST_H_*/
