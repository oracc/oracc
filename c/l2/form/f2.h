#ifndef F2_H_
#define F2_H_

#include <psdtypes.h>
#include "npool.h"
#include "words.h"

struct sig_context;

#define F2_FLAGS_PSU_STOP    0x000001
#define F2_FLAGS_PSU_SKIP    0x000002
#define F2_FLAGS_LEM_NEW     0x000004
#define F2_FLAGS_SHADOW      0x000008
#define F2_FLAGS_COF_NEW     0x000010
#define F2_FLAGS_INVALID     0x000020
#define F2_FLAGS_CF_QUOTED   0x000040
#define F2_FLAGS_NO_FORM     0x000080
#define F2_FLAGS_PARTIAL     0x000100
#define F2_FLAGS_SAME_REF    0x000200
#define F2_FLAGS_COF_HEAD    0x000400
#define F2_FLAGS_COF_TAIL    0x000800
#define F2_FLAGS_IS_PSU      0x001000
#define F2_FLAGS_NGRAM_MATCH 0x002000
#define F2_FLAGS_NGRAM_KEEP  0x004000
#define F2_FLAGS_LEM_BY_NORM 0x008000
#define F2_FLAGS_NORM_IS_CF  0x010000
#define F2_FLAGS_NEW_BY_PROJ 0x020000
#define F2_FLAGS_NOT_IN_SIGS 0x040000
#define F2_FLAGS_EXTENDED_OK 0x080000
#define F2_FLAGS_NEW_BY_LANG 0x100000
#define F2_FLAGS_COF_INVALID 0x200000
#define F2_FLAGS_FROM_CACHE  0x400000

/* This includes '<' because it is the delimiter for post-F2 
   predicates, and ' ' and '\t' because after ']' no spaces are
   allowed.
 
   N.B.: @, % and : are purposely not included in this set. The
   only entries in this list are those which can delimit fields
   in a signature after the CF[GW]POS.
 */
#define F2_FIELD_ENDS_STR	"$#/+*\\< \t"

struct f2
{
  /* Administrative fields */
  const Uchar *file;
  int lnum;
  Unsigned32 flags;
  /* needs thinking; record instance-explicit in f2 of ilem_form; 
     record matches in f2 from sigset; reset sigset f2 match
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

  /* Data structure for use by wordset lookup */
  struct w2_set *words;

  /* Array of components for COFs & PSUs; N.B.: use flags to 
   * determine if COF or PSU.
   */
  struct f2 **parts;
  uintptr_t cof_id;
};

#include <stdio.h>
#include "sigs.h"

struct ilem_form;
extern int f2_parse(const Uchar *file, size_t line, Uchar *lp, struct f2 *f2p, Uchar **psu_sense,
		    struct sig_context *scp);
extern void f2_attr(FILE *f_f2, const char *name, const char *value);
extern void f2_attr_i(FILE *f_f2, const char *name, int value);
extern void f2_serialize_form(FILE *f_f2, struct f2 *f);
extern void f2_serialize_form_2(FILE *f_f2, struct ilem_form *f);
extern unsigned char *f2_sig(struct xcl_context *xcp, struct ilem_form *ifp, struct f2 *fp);
extern unsigned char *f2_psu_sig(struct xcl_context *xcp, struct f2 *fp);
extern int f2_alias(struct sig_context *scp, struct f2 *fp, struct f2 *ref_fp);

extern void f2_init(void);
extern void f2_term(void);

extern int f2_form_signs(const unsigned char *f1, const unsigned char *f2);
extern int f2_extreme_alias(struct sig_context *scp, struct f2 *fp, struct f2 *ref_fp);

#endif/*F2_H_*/
