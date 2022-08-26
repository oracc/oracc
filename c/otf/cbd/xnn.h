#ifndef _XNN_H
#define _XNN_H

struct xnn_xname
{
  char *pname;
  char *qname;
};

struct xnn_attr
{
  char *valpair[2];
  char *renpair[2];
};

struct xnn_nstab {
  int prefix;
  const char *ns;
};

#endif/*_XNN_H*/
