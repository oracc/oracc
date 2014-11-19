#ifndef SYMBOLATTR_
#define SYMBOLATTR_

struct symbolattr
{
  char *symbol;
  char *qualified_id;
  char *pname;
};

extern void symbolattr_init(void);
extern struct symbolattr *symbolattr_get(const char *textid, const char *sym);
extern void symbolattr_put(const char *textid, const char *sym, const char *idp, const char *pname);
extern void symbolattr_term(void);

#endif/*SYMBOLATTR_*/
