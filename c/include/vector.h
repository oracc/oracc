/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: vector.h,v 0.4 1997/09/08 14:49:58 sjt Exp $
*/

#ifndef VECTOR_H_
#define VECTOR_H_ 1

#define vec_create() NULL

extern const char *vec_sep_str;

extern char **vec_copy (size_t vecc, char **vecv, size_t *vecc_p);
extern char **vec_find (char **vec, char *str);
extern char **vec_from_str (char * str, char *(*tok)(char *), size_t *vecsize_p);
extern char *vec_to_str (char **vec, size_t count, char *sep);
extern void vec_sep_push (const char *);
extern void vec_sep_pop (void);
extern char **vec_add (char ** vec, char *ptr);
extern void vec_free (char **vec);
extern char ** vec_read (const char *fname, FILE *fp, size_t *vecsize_p);
extern void vec_write (const char *fname, FILE *fp, const char **argv);
extern size_t vec_len (char *const*vec);
#endif /*VECTOR_H_*/
