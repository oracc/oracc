#ifndef XNN_H_
#define XNN_H_

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
  const char *uri;
};

struct xnn_data {
  struct xnn_xname *enames;
  struct xnn_xname *anames;
  struct xnn_nstab *nstab;
};

#endif/*XNN_H_*/
