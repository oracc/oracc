#ifndef MESG_H_
#define MESG_H_

struct Mloc
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
  char *file;
  void *user;
};

typedef struct Mloc Mloc;

extern const char *phase;

extern void mesg_append(char *a);
extern void mesg_init(void);
extern void mesg_print(FILE *fp);
extern void mesg_term(void);
extern char *mesg_loc(Mloc *locp);
extern void mesg_err(Mloc *locp, char *s);
extern void mesg_verr(Mloc *locp, char *s, ...);
extern void mesg_averr(Mloc *locp, char *s, va_list ap);
extern void mesg_print(FILE *fp);

#endif/*MESG_H_*/

