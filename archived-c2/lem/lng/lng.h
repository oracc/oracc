#ifndef LANG_H_
#define LANG_H_

#include "hash.h"
#undef C
#define C(x) x,
#define T_MODES  C(m_graphemic)C(m_normalized)C(m_morphology)C(m_alphabetic)
enum t_modes { T_MODES modes_top };
#undef C

enum t_langmode { m_logo , m_lang, m_sign };

enum langcode
{ 
  c_akk , c_arc , c_elx , c_hit , 
  c_peo , c_qam , c_qcu , c_qeb , 
  c_xhu , c_qpc , c_qpe , c_xur , 
  c_sux , c_uga , c_qpn , c_grc ,
  c_xlc , c_xld , c_xlu , c_hlu ,
  c_imy , c_plq , c_qse , c_qca ,
  c_egy , c_xht , c_qur , c_qka ,
  c_count , c_none
};

#define LF_BASE 0x001
#define LF_NORM 0x002
#define LF_SAC  0x004
#define LF_LOGO 0x008

struct langcore
{
  const char *name; 		/* lang code, e.g., sux */
  const char *script;		/* default script code for language */
  const char *altlang;  	/* alternate lang for logograms/lang switch, 
				   e.g., akk */
  enum t_langmode uppercase;	/* is uppercase logogram or unknown sign? */
  enum t_langmode underscore;   /* is underscore logogram or language switch? */
  enum langcode code;		/* enum code for array access */
  int features;
};

struct langmode
{
  const char *name;
  enum t_modes mode;
};

struct langrtl
{
  const char *name;
  int yes;
};

struct lang_tag
{
  char *tag;
  char *lang;
  char *xlang;
  const char *script;
  char *region;
  char *variant;
  char *x;
};

/* A lang_context is the combination of core language data and information
   from a language switch of any kind.  Each distinct combination of 
   these data can be serialized as a distinct langtag, and these are
   used as keys so that the loadable data for each lang_context are only
   loaded once (and may be passed from one lang_context to another where
   appropriate).

   Lang_Context data is project-local.
 */
struct lang_context
{
  struct langcore *core; 	/* original core language data, i.e., defaults */
  struct lang_tag *tag;
  const char *altlang;		/* alternate lang if different from default */
  const char *fulltag; 		/* this is composed from the post-inheritance tag */
  const char *script;		/* script for this instance, either from tag or core */
  const char *signlist;	     	/* the signlist loaded for this language for this
				   project */

  Hash *values;	     	/* sign values for the language */
  Hash *snames;		/* canonical sign names */

  List *sigsets;		/* this is a pointer to the list of sigsets
				   for a language-within-a-project that is
				   stored in the sig_context.langs */
  struct sigset *defsigs;	/* this is the default sigset for use with the language;
				   it is not guaranteed to be set: it only gets set when 
				   the sigset is loaded in sigs_lookup
				 */
  struct sigset *runsigs;	/* this is the sigset for items that are
				   generated/registered during the run; it is
				   also included in the sigsets list */
  enum t_modes  mode;		/* parsing mode for text */

  enum t_langmode uppercase;	/* is uppercase logogram or unknown sign? */
  enum t_langmode underscore;   /* is underscore logogram or language switch? */

  struct charset *cset;		/* charset for use in validating text */
  int charset_init;		/* did we do cset initialization yet? */
  struct proj_context *owner;
  int used_in_text;
};

struct langatf
{
  const char *name;
  const char *lang;
};

extern struct lang_context *global_lang;
extern struct lang_context *text_lang;
extern struct lang_context *curr_lang_ctxt;
extern struct lang_context *logo_lang;

extern struct langcore *lang_core(register const char *str,register size_t len);
extern struct langcore *langcore_of(const char *lang);
extern struct langmode *lang_mode(register const char *str,register size_t len);
extern struct langrtl *langrtl(register const char *str,register size_t len);
extern struct langatf *lang_atf(register const char *str, register size_t len);
extern void lng_init(void);
extern void lng_term(void);

extern void langtag_init(void);
extern void langtag_term(void);

extern void set_uscore_mode(const char *lang, enum t_langmode mode);

extern const char *langtag_compose(struct lang_tag *def, const char *lang,
				   const char *xlang, const char *script, const char *x);
extern struct lang_tag *langtag_create(struct lang_context *base,
				       const char *tag, int *taglen, 
				       const char *file, int lnum);
extern struct lang_context *lang_switch(struct lang_context *curr, 
					const char *tag, int *taglenp,
					const char *file, int lnum);

extern struct lang_context *lang_push(struct lang_context*);
extern struct lang_context *lang_pop(void);
extern void lang_reset(void);
extern void langtag_inherit(struct lang_tag *to, struct lang_tag *from);
extern void lang_term(void);

extern void texttag_init(void);
extern char *texttag_langs(void);
extern void texttag_register(const char *tag);
extern void texttag_term(void);
extern char *tag_no_script(const char *tag);

extern const char *fwhost;

#endif /*LANG_H_*/
