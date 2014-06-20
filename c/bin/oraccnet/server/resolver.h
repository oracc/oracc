#ifndef RESOLVER_H_
#define RESOLVER_H_

#ifdef __APPLE__
#define LOCALE "en_GB.UTF-8"
#else
#define LOCALE "en_GB.utf-8"
#endif

#include "ccgi/ccgi.h"

enum e_builtins  { B_NONE, B_ATF, B_AS, B_DATESTAMP, B_DOC, B_FIND, 
		   B_ITEMMAIN, B_ITEMSIDE, B_OUTLINE, B_RESULTS, B_RPC, 
		   B_SIG, B_XFORMS, B_XIS, B_RUNTIME, B_SL, B_OAS
};
enum e_component { C_NONE, C_RES, C_SEL, C_VIEW, C_FORMAT, C_UI };
enum e_resource  { R_NONE, R_PQX, R_WORD, R_ENTITY, R_LANG, R_KEY, R_LIST };
enum e_selection { S_NONE, S_FULL, S_CAT, S_TLIT, S_XLAT, S_TLIT_XLAT, 
		   S_IMAGE, S_THUMB, S_PHOTO, S_LINE, S_DETAIL,
		   S_SCORE, S_BLOCK, S_SIGN, S_VALS, S_HOMOPHONES, 
		   S_COMPOUNDS, S_CONTAINER, S_CONTAINED
};
enum e_view { V_NONE, V_CUNEIFIED, V_PROOFING, V_PROJECT, V_PROJECT_TYPE };
enum e_format { F_NONE, F_CATF, F_CSV, F_HTML, F_RDF, F_TEI, F_TXT, F_XML };
enum e_ui { U_NONE, U_MINI };

struct component
{
  enum e_component type;/* the URI component type */ 
  int value; 		/* essentially a union of e_resource, 
			   e_selection, e_view, e_format, e_ui */
  const char *text; 	/* the URI component text */
  const char *replace;  /* a replacement for the URI component text, to be used when
			   calling programs, e.g., when a PQX is virtualized text
			   will be P123456 but replace will be saao/saa10:P123456 */
  int index; 		/* the index of the component in the URI */
  int count; 		/* number of elements, e.g., count of PQX items in list */
};

struct seltab
{
  const char *name;
  enum e_selection val;
};
extern struct seltab *selections(register const char *str, register unsigned int len);

struct builtinstab
{
  const char *name;
  enum e_builtins type;
};
extern struct builtinstab *builtins(register const char *str, register unsigned int len);
extern struct builtinstab *legacy (register const char *str, register unsigned int len);

struct h_pqxtab
{
  const char *name;
  void (*func)(const char *,struct component*);
};
extern struct h_pqxtab *pqx_h_tab(register const char *str, register unsigned int len);

extern void or_builtins(void);
extern void or_gdf(void);
extern void uri_patterns(void);


extern const char *script_url;

extern const char **elements, *project, *query_string, *mode, *docroot, *xmlroot;
extern int nelements;

extern const char *make_find_phrase(const char *xkey, const char *phrase, int uncomma);
extern const char *cgi_arg(const char *arg, const char *val);
extern const char *create_session(void);
extern void do400(const char *message);
extern void do404(void);
extern void find(const char *project, const char *phrase);
extern void listdir(const char *arg);
extern void print_hdr(void);
extern void print_hdr_text(void);
extern void print_hdr_xml(void);
extern void print_xforms_pi(void);

extern void adhoc(const char *adhoc_texts, const char *line_id, const char *frag_id);
extern void cat_file(const char *file);
extern void cat_html_file(const char *file);
extern void cat_index_html(const char *dir);
extern void corpus(void);
extern void catentry(const char *item);
extern void p3(const char *project);
extern void p3_asearch_results(const char *project, const char *tmpdir);
extern void p3_exec(CGI_varlist *vl);
extern void p3_fake_srch(const char *project, const char *idx, const char *srch);
extern char *p3tempdir(void);
extern void progexec(const char *prog);
extern const char *map_PQX(const char *text, int ncomma);
extern void proof(const char *item);
extern void score_block(const char *item);
extern void sig(const char *sig);
extern void sigfixer_html(const char *file);
extern void tei(const char *item);
extern void list(const char *list);
extern void sed_project(const char *xml, const char *session);
extern void statistics(void);
extern void xis(const char *xlang, const char *xid);

extern void pqx_handler(struct component *);

extern void h_pqx_cuneified_html(const char *project, struct component *components);
extern void h_pqx_detail(const char *project, struct component *components);
extern void h_pqx_html(const char *project, struct component *components);
extern void h_pqx_html_pager(const char *project, struct component *components);
extern void h_pqx_image(const char *project, struct component *components);
extern void h_pqx_images(const char *project, struct component *components);
extern void h_pqx_line(const char *project, struct component *components);
extern void h_pqx_cat_html(const char *project, struct component *components);
extern void h_pqx_cat_xml(const char *project, struct component *components);
extern void h_pqx_photo(const char *project, struct component *components);
extern void h_pqx_proofing_html(const char *project, struct component *components);
extern void h_pqx_score_html(const char *project, struct component *components);
extern void h_pqx_thumb(const char *project, struct component *components);
extern void h_pqx_tlit_html(const char *project, struct component *components);
extern void h_pqx_tlit_tei(const char *project, struct component *components);
extern void h_pqx_tlit_xml(const char *project, struct component *components);
extern void h_pqx_trans_html(const char *project, struct component *components);
extern void h_pqx_trans_xml(const char *project, struct component *components);
extern void h_pqx_xml(const char *project, struct component *components);
extern void h_pqx_view(const char *project, struct component *components);

extern void show_components(struct component *components);

extern const char *oracc_home(void);
extern const char *oracc_var(void);
extern char *or_find_www_file(const char *project, const char *dir, const char *basename, const char *ext);
extern char *or_find_pqx_file(const char *project, const char *pqid, const char *ext);
extern char *or_find_pqx_xtr(const char *project, const char *pqid, const char *code, const char *lang);

extern void sl(struct component *);

extern int _is_ncname(const char *name);

extern void cuneify(void);

extern void xmlrpc(void);

#endif/*RESOLVER_H_*/
