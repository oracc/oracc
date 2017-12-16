#ifndef SIGS_H_
#define SIGS_H_
#include <psdtypes.h>
#include <hash.h>
#include <ngram.h>
#include <psus.h>

struct ilem_form;

struct sig_context
{
  struct npool *pool;
  struct xpd *xpd;
  Hash_table *known_bad;/* avoid giving lots of warnings for 
			    one file */
  Hash_table *langs; 	/* A hash of langtags, values are Lists 
			   whose data nodes are struct sigset */

  const char *aliases_file;	/* source of aliases */
  struct sas_info *aliases;	/* loaded aliases for Sumerian */  

  struct mb *mb_f2s;
  struct mb *mb_f2ps;		/* COF support uses this */
  struct mb *mb_ilem_forms;
  struct mb *mb_sigsets;
  struct mb *mb_sigs;
  struct xcl_context *xcp;
  int fuzzy;
};

struct sigset
{
  const Uchar *project;
  const Uchar *lang;
  struct langcore *core;
  const Uchar *file;
  Uchar *fmem;
  Uchar **lines;
  Hash_table *forms; /* A hash of orthographic forms, values
			are Lists whose data nodes are struct sig.
			This structure is what the sig file gets
			loaded into.  The struct sig nodes are not
			filled at first, but only when the form is
			the target of a lookup.
		      */
  Hash_table *norms; /* A hash of citation forms and/or
			normalizations, structured like the forms
			hash and pointing to the same struct sig
			instances as are referenced in the forms
			hash 
		      */
  Hash_table *bigrams_hash;
  Hash_table *collos_hash;
  Hash_table *mdsets_hash;
  struct sigset *cache;
  struct NL_context *bigrams;
  struct NL_context *collos;
  struct NL_context *psus;
  struct sig_context *owner;
  int loaded;
};

struct sig
{
  const Uchar *sig;
  const Uchar *literal; /* used by cache */
  struct sigset *set;
  struct f2 *f2p;
  struct ilem_form *ifp;
  struct w2_set *w2p;
  Unsigned32 rank;
  Unsigned32 freq;
  Unsigned32 pct;
  struct sig *next;
  int count; /* number of linked 'next' nodes */
  int lnum;
};

typedef struct sig const * const *look_test_func(struct xcl_context *, struct ilem_form *, 
						 struct sigset *, int *);

enum sig_look_t { sig_look_check , sig_look_new , sig_look_cache };
struct siglook
{
  const char *name;
  look_test_func *test;
  enum sig_look_t type;
};

extern struct siglook look_cache;

extern int verbose, qpn_mode;

extern struct siglook global_sig_look_struct;

extern struct sig_context *sig_context_init(void);
extern void sig_context_term(void);
extern List *sig_context_register(struct sig_context *scp, 
				  const char *project, 
				  const char *lang,
				  int auto_ok);
extern int sig_load_set(struct sig_context*scp, struct sigset *sp);
extern List *sig_autoload_sets(struct sig_context *scp,
			       const char *project,
			       const char *lang);
extern void sig_check(struct xcl_context *xcp);
extern void sig_new(struct xcl_context *xcp);
extern void sig_show_insts(struct xcl_context *xcp);

extern const unsigned char *sigs_field_begin(const Uchar *lp, const Uchar *field);
extern const unsigned char *sigs_field_end(const Uchar *lp);
extern void sig_context_langs(struct sig_context *scp, const char *langs);
extern void sigs_lookup(struct xcl_context *xcp, struct xcl_l *l, struct siglook *look);
extern void sigs_l_check(struct xcl_context *xcp, struct xcl_l *l);

extern void sigs_cache_add(struct ilem_form *fp,
			   struct sig const *const *sigs);

extern struct sig const * const *sigs_cache_find(struct sigset *sp, struct ilem_form*fp);

extern void setup_ilem_form(struct sig_context *scp, struct xcl_l *l, unsigned char*pinst);

extern int field_ok(const Uchar *s1, const Uchar *s2);
extern int cfnorm_ok(struct f2 *f1, struct f2 *f2);
extern int posepos_ok(struct f2 *f1, struct f2 *f2);
extern int sense_ok(struct f2 *f1, struct f2 *f2, int gw_wild);
extern int f2_test(struct f2 *f1, struct f2 *f2);
extern int f2_test_no_sense(struct f2 *f, struct f2 *f2);
extern void sigs_cof_finds(struct xcl_l *l);
extern void sigs_cof_init(void);
extern void sigs_cof_term(void);
extern void sigs_warn(struct xcl_context *xcp, struct xcl_l *l, 
		      struct siglook *look, struct ilem_form *fp);
extern struct sigset *sig_new_context_free_sigset(void);
extern int sigs_new_sig(struct xcl_context *xcp, struct ilem_form *fp);
extern struct sig const * const *sigs_early_sig(struct xcl_context *xcp, 
						struct ilem_form *fp);
extern void sigs_load_one_sig(struct sig_context*scp, struct sigset *sp, 
			      const unsigned char *sig, int lnum, struct ilem_form*ifp,
			      int rankflag);
extern struct sig const * const *sigs_inst_in_sigset(struct xcl_context *xcp, 
						     struct ilem_form *ifp, 
						     struct sigset *sp, int *nfinds);
extern char *err_sig(struct ilem_form *fp);

#endif/*SIGS_H_*/
