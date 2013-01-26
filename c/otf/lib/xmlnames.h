#ifndef _XMLNAMES_H
#define _XMLNAMES_H
#define MAX_A_NAME  1
#define MAX_QA_NAME 1
struct xname
{
  char pname[MAX_A_NAME];
  char qname[MAX_QA_NAME];
};
enum a_type
{
  n_c,
  n_g,
  n_n,
  n_syn,
  n_x,
  n_xh,
  n_xml,
  n_xtr,
  a_enum_top
};
struct attr
{
  char *valpair[2];
  char *renpair[2];
};
enum e_type
{
  e_enum_top
};

struct nstab { enum a_type prefix; const char *ns; };

extern struct attr abases[];
extern struct nstab nstab[];
extern struct xname anames[];
extern struct xname enames[];

#endif /*_XMLNAMES_H*/
