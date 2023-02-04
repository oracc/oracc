/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: misc.h,v 1.1 1998/12/05 04:46:09 s Exp $
*/

#ifndef MISC_H_
#define MISC_H_ 1

#ifdef MSDOS
extern size_t fsize_t (const char *fn, FILE *fp);
#else
#define fsize_t fsize
#endif
#define getline xgetline
extern unsigned char * getline (FILE * f);
extern Boolean fcheckf (const char *fn1, const char *fn2);
extern Boolean fcheckf_list (char *fn1, char **fn2s);
extern off_t fsize (const char *fn, FILE *fp);

extern void filecopy (const char *from, const char *to);
extern int pct(int amount, int total);

extern unsigned char *slurp(const char *caller, const char *fname, ssize_t *fsizep);

#endif /*MISC_H_*/
