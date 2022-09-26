#ifndef MSGLIST_H_
#define MSGLIST_H_

typedef struct msgloc msgloc;
struct msgloc
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
  char *file;
  void *user;
};

extern const char *phase;

#ifndef ucp
#define ucp unsigned char *
#endif

extern void msglist_append(char *a);
extern void msglist_init(void);
extern void msglist_print(FILE *fp);
extern void msglist_term(void);
extern char *msglist_loc(msgloc *locp);
extern void msglist_err(msgloc *locp, char *s);
extern void msglist_verr(msgloc *locp, char *s, ...);
extern void msglist_averr(msgloc *locp, char *s, va_list ap);
extern void msglist_print(FILE *fp);
#endif/*MSGLIST_H_*/

