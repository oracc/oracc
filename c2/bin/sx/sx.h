#ifndef SX_H_
#define SX_H_
#include <asl.h>

enum sx_pos_e { sx_pos_init , sx_pos_inst , sx_pos_term };

struct sx_functions;

typedef void (sx_signlist_f)(struct sx_functions *,struct sl_signlist*,enum sx_pos_e);
typedef void (sx_letter_f)(struct sx_functions*,struct sl_signlist*,struct sl_letter*,enum sx_pos_e);
typedef void (sx_group_f)(struct sx_functions*,struct sl_signlist*,struct sl_group*,enum sx_pos_e);
typedef void (sx_sign_f)(struct sx_functions*,struct sl_signlist*,struct sl_inst*,enum sx_pos_e);
typedef void (sx_list_f)(struct sx_functions*,struct sl_signlist*,struct sl_inst*,enum sx_pos_e);
typedef void (sx_form_f)(struct sx_functions*,struct sl_signlist*,struct sl_inst*,enum sx_pos_e);
typedef void (sx_value_f)(struct sx_functions*,struct sl_signlist*,struct sl_inst*,enum sx_pos_e);
typedef void (sx_notes_f)(struct sx_functions*,struct sl_signlist*,struct sl_inst*);
typedef void (sx_unicode_f)(struct sx_functions*,struct sl_signlist*,struct sl_unicode*);
typedef void (sx_String_f)(struct sx_functions*,struct sl_signlist*,unsigned const char *,enum sx_pos_e);
typedef void (sx_List_f)(struct sx_functions*,struct sl_signlist*,List *,enum sx_pos_e);

#include <stdio.h>

struct sx_functions
  {
    sx_signlist_f *	sll;
    sx_letter_f *	let;
    sx_group_f *	grp;
    sx_sign_f *		sgn;
    sx_form_f *		frm;
    sx_list_f *		lst;
    sx_value_f *	val;
    sx_value_f *	inh;
    sx_value_f *	qvs;
    sx_notes_f *	not;
    sx_unicode_f *	uni;
    FILE *fp;
    const char *fname;
  };


extern int identity_mode;
extern Hash *oids;
extern Hash *oid_sort_keys;

extern int toks_cmp(const void *a, const void *b);

extern void sx_xml(struct sl_signlist *sl);
extern void sx_xml_init(void);

extern void sx_compounds(struct sl_signlist *sl);
extern void sx_compound_digests(struct sl_signlist *sl);
extern void sx_homophones(struct sl_signlist *sl);
extern void sx_inherited(struct sl_signlist *sl);
extern void sx_listdefs(struct sl_signlist *sl, const char *listnames);
extern void sx_list_dump(struct sl_signlist *sl);
extern void sx_marshall(struct sl_signlist *sl);
extern void sx_qualified(struct sl_signlist *sl);
extern void sx_values_by_oid(struct sl_signlist *sl);
extern void sx_values_parents(struct sl_signlist *sl);
extern void sx_values_parents_dump(struct sl_signlist *sl);

extern void sx_walk(struct sx_functions *f, struct sl_signlist *sl);

extern struct sx_functions *sx_w_asl_init(FILE *fp, const char *fname);
extern struct sx_functions *sx_w_jsn_init(FILE *fp, const char *fname);
extern struct sx_functions *sx_w_xml_init(FILE *fp, const char *fname);
extern struct sx_functions *sx_sll_init(FILE *fp, const char *fname);
extern void sx_s_sll(FILE *f, struct sl_signlist *sl);

#endif/*SX_H_*/
