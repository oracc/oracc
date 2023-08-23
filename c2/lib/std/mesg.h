#ifndef MESG_H_
#define MESG_H_

#include <stdio.h>
#include <list.h>

struct Mloc
{
  const char *file;
  int line;
  void *user;
};

typedef struct Mloc Mloc;

extern const char *phase;
extern int mesg_no_loc;

extern void mesg_append(const char *a);
extern void mesg_init(void);
extern void mesg_print(FILE *fp);
extern void mesg_term(void);
extern char *mesg_loc(Mloc *locp);
extern Mloc *mesg_mloc(const char *file, size_t line);
extern void mesg_err(Mloc *locp, const char *s);
extern void mesg_verr(Mloc *locp, const char *s, ...);
extern void mesg_averr(Mloc *locp, const char *s, va_list ap);
extern void mesg_prefix(const char *p);
extern void mesg_print(FILE *fp);
extern void mesg_print2(FILE *fp, List *mlist);
extern List *mesg_retrieve(void);

extern void mesg_notice(const char *file, int ln, const char *str);
extern void mesg_vnotice(const char *file, int ln, const char *str, ...);
extern void mesg_warning(const char *file, int ln, const char *str);
extern void mesg_vwarning(const char *file, int ln, const char *s, ...);
extern void mesg_avwarning(const char *file, int ln, const char *str, va_list ap);

extern int mesg_remove_error(const char *file, int line, const char *str);

extern void mloc_init(void);
extern void mloc_term(void);
extern Mloc *mloc_file_line(const char *file, int line);
extern Mloc *mloc_mloc(Mloc *arg_ml);

extern int mesg_status(void);
extern void mesg_status_ignore_one(void);
extern void mesg_status_reset(void);

/* Support for Yacc -- include mesg.h before xxx.tab.h and add
 *
 * #define XXXLTYPE Mloc
 *
 * Before including xxx.tab.h--XXX is API, e.g., GDL
 *
 */

#define YYLTYPE_IS_DECLARED 1
#undef YYLLOC_DEFAULT
#define YYLLOC_DEFAULT(Current, Rhs, N)                                 \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).line   = YYRHSLOC (Rhs, 1).line;        		\
          (Current).file         = YYRHSLOC (Rhs, N).file;              \
	}                                                               \
      else                                                              \
        {                                                               \
          (Current).line   = YYRHSLOC (Rhs, 0).line;              	\
          (Current).file         = YYRHSLOC (Rhs, 0).file;     		\
    }  								        \
    while (0)


#endif/*MESG_H_*/
