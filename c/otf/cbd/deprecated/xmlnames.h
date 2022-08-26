#ifndef _XMLNAMES_H
#define _XMLNAMES_H

struct xn_xname
{
  char *pname;
  char *qname;
};

struct xn_attr
{
  char *valpair[2];
  char *renpair[2];
};

struct xn_nstab {
  int prefix;
  const char *ns;
};

#endif/*_XMLNAMES_H*/
