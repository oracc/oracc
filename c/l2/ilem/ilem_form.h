#ifndef L2_FORM_H_
#define L2_FORM_H_

#include "f2.h"

#define ILEM_FLAG_SPARSE_SKIP 0x20

#define M_CF		0x0001
#define M_MIN		M_CF
#define M_GW		0x0002
#define M_NORM0		0x0004
#define M_SENSE		0x0008
#define M_FORM		0x0010
#define M_POS		0x0020
#define M_CONT		0x0040
#define M_DISAMB	0x0080
#define M_EPOS		0x0100
#define M_BASE		0x0200
#define M_MORPH		0x0400
#define M_MORPH2       	0x0800
#define M_RWS		0x1000
#define M_AUGMENT	0x2000
#define M_MAX		M_AUGMENT

#define M_N_SHIFT 	15
#define N2M(x)		(x>>M_N_SHIFT)

#define NEW_CF		(M_CF<<M_N_SHIFT)
#define NEW_MIN		NEW_CF
#define NEW_GW		(M_GW<<M_N_SHIFT)
#define NEW_NORM0	(M_NORM0<<M_N_SHIFT)
#define NEW_SENSE	(M_SENSE<<M_N_SHIFT)
#define NEW_FORM	(M_FORM<<M_N_SHIFT)
#define NEW_POS		(M_POS<<M_N_SHIFT)
#define NEW_CONT	(M_CONT<<M_N_SHIFT)
#define NEW_DISAMB	(M_DISAMB<<M_N_SHIFT)
#define NEW_EPOS	(M_EPOS<<M_N_SHIFT)
#define NEW_BASE	(M_BASE<<M_N_SHIFT)
#define NEW_MORPH	(M_MORPH<<M_N_SHIFT)
#define NEW_RWS		(M_RWS<<M_N_SHIFT)
#define NEW_AUGMENT	(M_AUGMENT<<M_N_SHIFT)
#define NEW_MAX		NEW_AUGMENT

#define M_AMBIG 	(NEW_MAX<<1)

#define M_MASK (M_CF|M_GW|M_NORM0|M_SENSE|M_FORM|M_POS|M_EPOS|M_CONT|M_DISAMB|M_AMBIG|M_BASE|M_MORPH|M_MORPH2|M_RWS)

#define N_RESULT(r) ((r) & ~M_MASK)

struct lem_instance
{
  const Uchar *file;
  size_t line;
  const Uchar *literal;
  struct lem_single *lem;
};

struct lem_single
{
  const Uchar *literal;
  struct f2 f2;
  Unsigned32 explicit;

  /*  struct lem_adjunct *adjuncts; */
  struct ilem_para *ante_para;
  struct ilem_para *post_para;
  struct lem_single *alt; /* ambiguity handling */
  struct lem_single *lem; /* COF handling */

  int instance_flags;   /* 0x01 == initial '!' == stop PSU processing 
			 * 0x02 == initial '-' == ignore during PSU processing
			 * 0x04 == initial '+' == new info in lemm
			 * 0x08 == initial '`' == instance is a shadow lem
			 * 0x10 == internal use by xffharvest -n; indicates
			 *           that the form is part of a COF sequence
			 *           with one or more new elements
			 * 0x20 == unlemmatized because it's in a sparse-skip field
			 */
};

struct ilem_form
{
  struct f2 f2;
  char *xml_id;   /* unique identifier for this form within its forms file */
  char *literal;  /* associated literal lemmatization info (content of #lem line) */
  char *sublem;   /*  */
  char *ref;      /* word ref of form instance */
  char *aform;    /* aliased form */
  char *abase;    /* aliased Sumerian base, e.g., e3 when translit has ed2 */
  char *onorm;    /* form with orthographic normalization */
  char *psu_sense;/* PSU senses can be selected on the head component;
		       given with +=SENSE
		       must come after //SENSE
		   */
  char *restrictor;/* lexical restrictor on sense, e.g. Sz/t form in Akk. */
  char *pref;     /* prefix (for Sumerian) */
  char *augment;  /* augmentation string, e.g., +.*ra */
  char *fdisamb;  /* form disambiguation string, e.g., \k */
  char *sdisamb;  /* sense disambiguation string, e.g., \t */
  char *status;   /* status, i.e., bad , good , auto */
  char *rws_cf;   /* Citation Form to use in glossaries of the form's RWS */
  char *file;	  /* file name for error messages */
  int newflag;		/* 1 = form is new (input began with +) */
  int invalid;		/* 1 = entry parsed as invalid by ATF processor */
  size_t lnum;		/* line number for error messages */
  int freq;		/* frequency of form in corpus */
  int pct;		/* percentage frequency of this form in 
			   homographous forms */
  char *parts;          /* prebuilt parts entry for use in xff2cbd */
  char *lrefs;          /* lemma references for compounds */
  struct ilem_form *ambig;   /* chains to next form in ambiguous analysis */
  size_t acount;	/* count of ambiguous forms; only valid in first form 
			   in chain */
  struct ilem_form *multi;   /* chain of multiple lemmata written in a single form */
  int mcount;		/* count of multiple lemmata; 
			   0 = not a multi
			   >0 = master node of a multi
			   <0 = slave node of a multi
			*/
  struct ilem_form *compound;/* a compound CBD entry has a form for the compound which
			   is linked to the master, then compound
			   elements N+1... are linked in as multis */
  int is_part;		/* this instance form is part of a compound,
			   so don't index it; -1 means this is the
			   compound form itself */
  int result; 		/* inform caller if find was successful */
  struct ilem_form **finds;  /* forms that were found in lookup */
  int fcount;		/* count of entries in finds */
  struct sigset *sp;    /* sigset finds came from */
  struct siglook *look; /* lookup type the finds resulted from */

  
  int transient;        /* used in cache/memory management */
  int explicit;         /* uses lem_bits mechanism to store what was specified
			   explicitly in a user-specified lemmmatization:
			     for each item that is explicitly given the M_XXX flag is
			       set
			     for each item that is quoted the NEW_XXX flag is set
			 */
  int default_sense;    /* 1 if the GW is an OK match for this SENSE */
  int instance_flags;   /* 0x01 == initial '!' == stop PSU processing 
			 * 0x02 == initial '-' == ignore during PSU processing
			 * 0x04 == initial '+' == new info in lemm
			 * 0x08 == initial '`' == instance is a shadow lem
			 * 0x10 == internal use by xffharvest -n; indicates
			 *           that the form is part of a COF sequence
			 *           with one or more new elements
			 * 0x20 == unlemmatized because it's in a sparse-skip field
			 */
  const char *type;
  struct form_MD*meta;
  struct dict_ID*dict;
  struct prop *props;
  struct lang_context *lang;
};

#endif/*L2_FORM_H*/
