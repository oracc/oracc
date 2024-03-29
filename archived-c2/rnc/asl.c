#ifndef NULL
#define NULL (char *)0
#endif
#include "xnn.h"
#include "ns-asl.h"
struct xnn_nstab asl_nstab[] = {
  { asl_n_g, "xmlns:g", "http://oracc.org/ns/gdl/1.0" },
  { asl_n_n, "xmlns:n", "http://oracc.org/ns/norm/1.0" },
  { asl_n_sl, "xmlns:sl", "http://oracc.org/ns/sl/1.0" },
  { asl_a_enum_top, NULL, NULL },
};

struct xnn_xname asl_anames[] =
{
  { "xmlns:g", "xmlns:g" },
  { "xmlns:n", "xmlns:n" },
  { "xmlns:sl", "xmlns:sl" },
  { "xmlns:xml", "xmlns:xml" },
  { "base", "base" },
  { "comment", "comment" },
  { "compoundonly", "compoundonly" },
  { "cpd-refs", "cpd-refs" },
  { "deprecated", "deprecated" },
  { "form", "form" },
  { "g:accented", "http://oracc.org/ns/gdl/1.0:accented" },
  { "g:delim", "http://oracc.org/ns/gdl/1.0:delim" },
  { "g:logolang", "http://oracc.org/ns/gdl/1.0:logolang" },
  { "g:pos", "http://oracc.org/ns/gdl/1.0:pos" },
  { "g:role", "http://oracc.org/ns/gdl/1.0:role" },
  { "g:status", "http://oracc.org/ns/gdl/1.0:status" },
  { "g:type", "http://oracc.org/ns/gdl/1.0:type" },
  { "g:utf8", "http://oracc.org/ns/gdl/1.0:utf8" },
  { "hex", "hex" },
  { "label", "label" },
  { "loc", "loc" },
  { "n", "n" },
  { "name", "name" },
  { "num", "num" },
  { "o", "o" },
  { "oid", "oid" },
  { "order", "order" },
  { "p", "p" },
  { "path", "path" },
  { "period", "period" },
  { "project", "project" },
  { "qm", "qm" },
  { "qv", "qv" },
  { "ref", "ref" },
  { "sexified", "sexified" },
  { "sort", "sort" },
  { "title", "title" },
  { "token", "token" },
  { "type", "type" },
  { "uncertain", "uncertain" },
  { "xml:id", "http://www.w3.org/XML/1998/namespace:id" },
  { "xml:lang", "http://www.w3.org/XML/1998/namespace:lang" },
  { NULL, NULL },
};
struct xnn_attr asl_abases[] =
{
  { { "base",NULL } , { "base",NULL } },
  { { "comment",NULL } , { "comment",NULL } },
  { { "compoundonly",NULL } , { "compoundonly",NULL } },
  { { "cpd-refs",NULL } , { "cpd-refs",NULL } },
  { { "deprecated",NULL } , { "deprecated",NULL } },
  { { "form",NULL } , { "form",NULL } },
  { { "g:accented",NULL } , { "http://oracc.org/ns/gdl/1.0:accented",NULL } },
  { { "g:delim",NULL } , { "http://oracc.org/ns/gdl/1.0:delim",NULL } },
  { { "g:logolang",NULL } , { "http://oracc.org/ns/gdl/1.0:logolang",NULL } },
  { { "g:pos",NULL } , { "http://oracc.org/ns/gdl/1.0:pos",NULL } },
  { { "g:role",NULL } , { "http://oracc.org/ns/gdl/1.0:role",NULL } },
  { { "g:status",NULL } , { "http://oracc.org/ns/gdl/1.0:status",NULL } },
  { { "g:type",NULL } , { "http://oracc.org/ns/gdl/1.0:type",NULL } },
  { { "g:utf8",NULL } , { "http://oracc.org/ns/gdl/1.0:utf8",NULL } },
  { { "hex",NULL } , { "hex",NULL } },
  { { "label",NULL } , { "label",NULL } },
  { { "loc",NULL } , { "loc",NULL } },
  { { "n",NULL } , { "n",NULL } },
  { { "name",NULL } , { "name",NULL } },
  { { "num",NULL } , { "num",NULL } },
  { { "o",NULL } , { "o",NULL } },
  { { "oid",NULL } , { "oid",NULL } },
  { { "order",NULL } , { "order",NULL } },
  { { "p",NULL } , { "p",NULL } },
  { { "path",NULL } , { "path",NULL } },
  { { "period",NULL } , { "period",NULL } },
  { { "project",NULL } , { "project",NULL } },
  { { "qm",NULL } , { "qm",NULL } },
  { { "qv",NULL } , { "qv",NULL } },
  { { "ref",NULL } , { "ref",NULL } },
  { { "sexified",NULL } , { "sexified",NULL } },
  { { "sort",NULL } , { "sort",NULL } },
  { { "title",NULL } , { "title",NULL } },
  { { "token",NULL } , { "token",NULL } },
  { { "type",NULL } , { "type",NULL } },
  { { "uncertain",NULL } , { "uncertain",NULL } },
  { { "xml:id",NULL } , { "http://www.w3.org/XML/1998/namespace:id",NULL } },
  { { "xml:lang",NULL } , { "http://www.w3.org/XML/1998/namespace:lang",NULL } },
  { { NULL,NULL } , { NULL,NULL } },
};
struct xnn_xname asl_enames[] =
{
  { "g:a", "http://oracc.org/ns/gdl/1.0:a" },
  { "g:b", "http://oracc.org/ns/gdl/1.0:b" },
  { "g:c", "http://oracc.org/ns/gdl/1.0:c" },
  { "g:d", "http://oracc.org/ns/gdl/1.0:d" },
  { "g:g", "http://oracc.org/ns/gdl/1.0:g" },
  { "g:m", "http://oracc.org/ns/gdl/1.0:m" },
  { "g:n", "http://oracc.org/ns/gdl/1.0:n" },
  { "g:nonw", "http://oracc.org/ns/gdl/1.0:nonw" },
  { "g:o", "http://oracc.org/ns/gdl/1.0:o" },
  { "g:p", "http://oracc.org/ns/gdl/1.0:p" },
  { "g:q", "http://oracc.org/ns/gdl/1.0:q" },
  { "g:r", "http://oracc.org/ns/gdl/1.0:r" },
  { "g:s", "http://oracc.org/ns/gdl/1.0:s" },
  { "g:v", "http://oracc.org/ns/gdl/1.0:v" },
  { "g:w", "http://oracc.org/ns/gdl/1.0:w" },
  { "n:s", "http://oracc.org/ns/norm/1.0:s" },
  { "n:w", "http://oracc.org/ns/norm/1.0:w" },
  { "sl:compoundonly", "http://oracc.org/ns/sl/1.0:compoundonly" },
  { "sl:form", "http://oracc.org/ns/sl/1.0:form" },
  { "sl:formproxy", "http://oracc.org/ns/sl/1.0:formproxy" },
  { "sl:i", "http://oracc.org/ns/sl/1.0:i" },
  { "sl:iheader", "http://oracc.org/ns/sl/1.0:iheader" },
  { "sl:images", "http://oracc.org/ns/sl/1.0:images" },
  { "sl:info", "http://oracc.org/ns/sl/1.0:info" },
  { "sl:inherited", "http://oracc.org/ns/sl/1.0:inherited" },
  { "sl:inote", "http://oracc.org/ns/sl/1.0:inote" },
  { "sl:iv", "http://oracc.org/ns/sl/1.0:iv" },
  { "sl:letter", "http://oracc.org/ns/sl/1.0:letter" },
  { "sl:list", "http://oracc.org/ns/sl/1.0:list" },
  { "sl:listdef", "http://oracc.org/ns/sl/1.0:listdef" },
  { "sl:lit", "http://oracc.org/ns/sl/1.0:lit" },
  { "sl:lref", "http://oracc.org/ns/sl/1.0:lref" },
  { "sl:name", "http://oracc.org/ns/sl/1.0:name" },
  { "sl:note", "http://oracc.org/ns/sl/1.0:note" },
  { "sl:pname", "http://oracc.org/ns/sl/1.0:pname" },
  { "sl:proof", "http://oracc.org/ns/sl/1.0:proof" },
  { "sl:q", "http://oracc.org/ns/sl/1.0:q" },
  { "sl:qs", "http://oracc.org/ns/sl/1.0:qs" },
  { "sl:sign", "http://oracc.org/ns/sl/1.0:sign" },
  { "sl:signlist", "http://oracc.org/ns/sl/1.0:signlist" },
  { "sl:signs", "http://oracc.org/ns/sl/1.0:signs" },
  { "sl:smap", "http://oracc.org/ns/sl/1.0:smap" },
  { "sl:sref", "http://oracc.org/ns/sl/1.0:sref" },
  { "sl:sys", "http://oracc.org/ns/sl/1.0:sys" },
  { "sl:sysdef", "http://oracc.org/ns/sl/1.0:sysdef" },
  { "sl:uage", "http://oracc.org/ns/sl/1.0:uage" },
  { "sl:ucun", "http://oracc.org/ns/sl/1.0:ucun" },
  { "sl:uname", "http://oracc.org/ns/sl/1.0:uname" },
  { "sl:unote", "http://oracc.org/ns/sl/1.0:unote" },
  { "sl:v", "http://oracc.org/ns/sl/1.0:v" },
  { NULL, NULL },
};
struct xnn_data asl_data = { asl_enames, asl_anames, asl_nstab };
