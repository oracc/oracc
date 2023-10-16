#ifndef FORM_H_
#define FORM_H_

#include <stdio.h>
#include <c2types.h>

#define Uchar unsigned char
#include "pool.h"

#define FORM_FLAGS_PSU_STOP    0x0000001
#define FORM_FLAGS_PSU_SKIP    0x0000002
#define FORM_FLAGS_LEM_NEW     0x0000004
#define FORM_FLAGS_SHADOW      0x0000008
#define FORM_FLAGS_COF_NEW     0x0000010
#define FORM_FLAGS_INVALID     0x0000020
#define FORM_FLAGS_CF_QUOTED   0x0000040
#define FORM_FLAGS_NO_FORM     0x0000080
#define FORM_FLAGS_PARTIAL     0x0000100
#define FORM_FLAGS_SAME_REF    0x0000200
#define FORM_FLAGS_COF_HEAD    0x0000400
#define FORM_FLAGS_COF_TAIL    0x0000800
#define FORM_FLAGS_IS_PSU      0x0001000
#define FORM_FLAGS_NGRAM_MATCH 0x0002000
#define FORM_FLAGS_NGRAM_KEEP  0x0004000
#define FORM_FLAGS_LEM_BY_NORM 0x0008000
#define FORM_FLAGS_NORM_IS_CF  0x0010000
#define FORM_FLAGS_NEW_BY_PROJ 0x0020000
#define FORM_FLAGS_NOT_IN_SIGS 0x0040000
#define FORM_FLAGS_EXTENDED_OK 0x0080000
#define FORM_FLAGS_NEW_BY_LANG 0x0100000
#define FORM_FLAGS_COF_INVALID 0x0200000
#define FORM_FLAGS_FROM_CACHE  0x0400000
#define FORM_FLAGS_DEF_FORM    0x0800000
#define FORM_FLAGS_DEF_ESENSE  0x1000000
#define FORM_FLAGS_DEF_ISENSE  0x2000000
#define FORM_FLAGS_READ_ONLY   0x4000000
#define FORM_FLAGS_CLEARED     0x8000000
#define FORM_FLAGS_NEWLITERAL  0x10000000

/* This includes '<' because it is the delimiter for post-FORM 
   predicates, and ' ' and '\t' because after ']' no spaces are
   allowed.
 
   N.B.: @, % and : are purposely not included in this set. The
   only entries in this list are those which can delimit fields
   in a signature after the CF[GW]POS.
 */
#define FORM_FIELD_ENDS_STR	"$#/+*\\< \t"

struct form
{
  /* Administrative fields */
  const Uchar *file;
  int lnum;
  Unsigned32 flags;
  
  /* needs thinking; record instance-explicit in form of ilem_form; 
     record matches in form from sigset; reset sigset form match
     before calling cfnorm_ok ? What are the persistency issues? */
  /*  Unsigned32 match; */  
  Uchar *sig;
  Uchar *tail_sig;
  Uchar *psu_ngram;

  /* The five static signature fields */
  const Uchar *cf;
  const Uchar *gw;
  const Uchar *sense;
  const Uchar *pos;
  const Uchar *epos;

  /* Additional fields which vary with instance and language */
  const Uchar *project;
  const Uchar *exo_project; /* used to save project that exosig comes from; output as exoprj when set */
  const Uchar *exo_lang; /* used to save lang of exosig; output as exolng when set */
  const Uchar *form;
  const Uchar *oform;
  const Uchar *lang;
  struct langcore *core;
  const Uchar *norm;
  const Uchar *base;
  const Uchar *cont;
  const Uchar *morph;
  const Uchar *morph2;
  const Uchar *stem;
  const Uchar *augment; /* need to store +.*ra until we get morphology from cbd */
  const Uchar *restrictor;
  const Uchar *rws;
  
  /* Array of components for COFs & PSUs; N.B.: use flags to 
   * determine if COF or PSU.
   */
  struct form **parts;
  uintptr_t cof_id;

  /* Store rank so it can persist across nlcp_rewrite */
  int rank;

  /* Support for arbitrary user data extensions */
  void *user;
};

typedef struct form Form;

extern int form_parse(const Uchar *file, size_t line, Uchar *lp, Form *formp, Uchar **psu_sense);
extern unsigned char *form_sig(Form *fp);
extern unsigned char *form_psu_sig(Form *fp);
extern int form_alias(Form *fp, Form *ref_fp);

extern void form_init(void);
extern void form_term(void);

extern int form_form_signs(const unsigned char *f1, const unsigned char *form);
extern int form_extreme_alias(Form *fp, Form *ref_fp);

extern void form_inherit(Form *inheritor_form, Form *from_form);
extern void form_clear(Form*f);

#endif/*FORM_H_*/
