#ifndef ORACCSYS_H_
#define ORACCSYS_H_

#include <wchar.h>
#include <wctype.h>
#include <sys/types.h>
#include <c2types.h>

extern unsigned char *slurp(const char *caller, const char *fname, ssize_t *fsizep);

#define	default_ext(fn,fext) new_ext(fn, fext, FALSE)
#define	force_ext(fn,fext) new_ext(fn, fext, TRUE)
#define	default_path(fn,fpath) new_path(fn, fpath, FALSE)
#define	force_path(fn,fpath) new_path(fn, fpath, TRUE)

extern  char *expand(const char *project,const char *pqid,const char *ext);
extern  char *expand_xtr (const char *project, const char *pqid, const char *code, const char *lang);
extern  char * find_ext(char *fn);
extern  char * find_path(char *fn);
extern  char * basename(const char *fn);
extern  char * extname(char *fn);
extern  char * filename(char *fn);
extern  char * pathname(char *fn);
extern  char * new_ext(char *fn, const char *fext, int force);
extern  char * new_ext_no_alloc(char *oldfile,const char *ext);
extern  char * new_path(char *fn, const char *fpath, int force);
extern const char *oracc_builds (void);
extern const char *oracc_home (void);
extern const char *oracc_project_home (void);
extern const char *oracc_var (void);

extern const unsigned char *utf_lcase(const unsigned char *s);
extern const unsigned char *utf_ucase(const unsigned char *s);
extern wchar_t utf1char(const unsigned char *src,size_t *len);
extern unsigned char *utf2atf(const unsigned char *src);
extern wchar_t* utf2wcs(const unsigned char *src, size_t *len);
extern int u_isupper(const unsigned char *g);
extern int u_islower(const unsigned char *g);
extern unsigned char *wcs2atf(wchar_t *wc, size_t len);
extern unsigned char *wcs2utf(const wchar_t*ws, size_t len);
extern wchar_t*wcs_lcase(wchar_t*ws);
extern wchar_t*wcs_ucase(wchar_t*ws);
extern wchar_t*wcs_undet(wchar_t*ws);
extern unsigned char *sexify(int n, const char *aszdisz);

extern uint32_t hex2int(char *hex);
extern unsigned char *uhex2utf8(unsigned const char *uhex);

extern void help (void);
extern int opts (int optchar, char *optarg);
extern void options (int argc, char *const *argv, const char *optstr);
extern void banner(void);
extern void usage(void);
extern int optind, opterr;
extern char *optarg;
extern void program_values(const char *name, int major, int minor, const char *usage, const char *copyright);

#endif/*ORACCSYS_H_*/
