/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: fname.h,v 0.3 1997/09/08 14:49:56 sjt Exp $
*/

#ifndef FNAME_H_
#define FNAME_H_ 1

#include <psdtypes.h>

#define	default_ext(fn,fext) new_ext(fn, fext, FALSE)
#define	force_ext(fn,fext) new_ext(fn, fext, TRUE)
#define	default_path(fn,fpath) new_path(fn, fpath, FALSE)
#define	force_path(fn,fpath) new_path(fn, fpath, TRUE)

#define oracc_builds 	c1_oracc_builds
#define oracc_home	c1_oracc_home
#define oracc_var	c1_oracc_var
#define oracc_project_home c1_oracc_project_home
#define new_ext		c1_new_ext
#define find_ext	c1_find_ext
#define new_ext_no_alloc c1_new_ext_no_alloc
#define new_path	c1_new_path
#define filename	c1_filename
#define find_path	c1_find_path
#define preslash	c1_preslash
#define expand		c1_expand
#define expand_xtr	c1_expand_xtr

extern  const char *expand(const char *project,const char *pqid,const char *ext);
extern const char *expand_xtr (const char *project, const char *pqid, const char *code, const char *lang);
extern  char *l2_expand(const char *project,const char *pqid,const char *ext);
extern char *l2_expand_xtr (const char *project, const char *pqid, const char *code, const char *lang);
extern  char * find_ext(char *fn);
extern  char * find_path(char *fn);
extern  char * drivename(char *fn);
extern  char * basename(const char *fn);
extern  char * extname(char *fn);
extern  char * filename(char *fn);
extern  char * pathname(char *fn);
extern  char * new_ext(char *fn, const char *fext, Boolean force);
extern  char * new_ext_no_alloc(char *oldfile,const char *ext);
extern  char * new_path(char *fn, const char *fpath, Boolean force);
extern  void slashes (char *fn);
extern  char *fname_from_prog(char *fn);
extern char * da_file (const char *base, const char *subdir, int subsub);
extern const char *oracc_builds (void);
extern const char *oracc_home (void);
extern const char *oracc_project_home (void);
extern const char *oracc_var (void);

#define cdl_home oracc_home
#define cdl_var  oracc_var

#endif /*FNAME_H_*/
