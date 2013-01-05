#ifndef _CHARSETS_H
#define _CHARSETS_H
#include "lang.h"
#include "inctrie.h"

struct lang_context;

struct charset_keys
{
  const char *key;
  const char *val;
};

struct charset
{
  const char *lang;
  enum langcode lcode;
  const char *mode;
  enum t_modes mcode;
  struct charset_keys *keys;
  struct inctrie *to_uni;
  int (*val)(wchar_t*,ssize_t);
  char *(*asc)(wchar_t*,ssize_t);
  wchar_t*(*u2u)(wchar_t*,ssize_t);
};

extern int chartrie_suppress_errors;
#define CHARSET_ERR_FUNC (void(*)(const char*,const char*,void*,const char *,size_t))chartrie_not

extern void charsets_init(void);
extern void charsets_term(void);
extern void subdig_init(void);
extern void subdig_term(void);
extern struct charset *get_charset(enum langcode lcode, enum t_modes mcode);
extern void chartrie_init(struct charset*cp);
extern void chartrie_not(const char *str, const char *sofar,struct charset *cp,
			 const char *f,size_t l);
extern void charset_init_lang(struct lang_context *lp);

extern const unsigned char *subdig(const unsigned char *str, 
				   const unsigned char *end);
extern const unsigned char *cpydig(const unsigned char *str, 
				   const unsigned char *end);

#endif /*_CHARSETS_H*/
