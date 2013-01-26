#ifndef NULL
#define NULL (char *)0
#endif
#include "xmlnames.h"
struct nstab nstab[] = {
  { n_c, "http://oracc.org/ns/cdf/1.0" },
  { n_g, "http://oracc.org/ns/gdl/1.0" },
  { n_n, "http://oracc.org/ns/norm/1.0" },
  { n_syn, "http://oracc.org/ns/syntax/1.0" },
  { n_x, "http://oracc.org/ns/xtf/1.0" },
  { n_xh, "http://www.w3.org/1999/xhtml" },
  { n_xml, "http://www.w3.org/XML/1998/namespace" },
  { n_xtr, "http://oracc.org/ns/xtr/1.0" },
};

struct xname anames[] =
{
  { "xmlns:c", "xmlns:c" },
  { "xmlns:g", "xmlns:g" },
  { "xmlns:n", "xmlns:n" },
  { "xmlns:syn", "xmlns:syn" },
  { "xmlns:x", "xmlns:x" },
  { "xmlns:xh", "xmlns:xh" },
  { "xmlns:xml", "xmlns:xml" },
  { "xmlns:xtr", "xmlns:xtr" },
};
struct attr abases[] =
{
  { { anames[0].qname,NULL } , { anames[0].pname,NULL } },
  { { anames[1].qname,NULL } , { anames[1].pname,NULL } },
  { { anames[2].qname,NULL } , { anames[2].pname,NULL } },
  { { anames[3].qname,NULL } , { anames[3].pname,NULL } },
  { { anames[4].qname,NULL } , { anames[4].pname,NULL } },
  { { anames[5].qname,NULL } , { anames[5].pname,NULL } },
  { { anames[6].qname,NULL } , { anames[6].pname,NULL } },
  { { anames[7].qname,NULL } , { anames[7].pname,NULL } },
};
struct xname enames[] =
{
};
