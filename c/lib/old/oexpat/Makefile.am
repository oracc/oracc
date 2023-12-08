## Process this file with automake to produce Makefile.in
##
#  Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
#  1997 under the GNU General Public License (see /psd/COPYING).
# 
#  $Id: Makefile.am,v 0.5 1997/09/08 13:02:15 sjt Exp $

noinst_LTLIBRARIES=libexpat.la
libexpat_la_SOURCES = \
  xmltok/xmltok.c \
  xmltok/xmlrole.c \
  xmlparse/xmlparse.c \
  xmlparse/hashtable.c

EXTRA_libexpat_la_SOURCES = \
  xmltok/asciitab.h \
  xmltok/iasciitab.h \
  xmltok/latin1tab.h \
  xmltok/nametab.h \
  xmltok/utf8tab.h \
  xmltok/xmldef.h \
  xmltok/xmlrole.h \
  xmltok/xmltok.h \
  xmltok/xmltok_impl.c \
  xmltok/xmltok_impl.h \
  xmltok/xmltok_ns.c \
  xmlparse/xmlparse.h \
  xmlparse/hashtable.h

AM_CPPFLAGS = -I@top_srcdir@/c/lib/expat/xmltok
