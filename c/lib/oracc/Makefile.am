## Process this file with automake to produce Makefile.in
##
#  Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
#  1997 under the GNU General Public License (see /psd/COPYING).
# 
#  $Id: Makefile.am,v 0.5 1997/09/08 13:02:15 sjt Exp $

include ../../../oracc-am-rules.txt

AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=liboracc.la

fname.c.in: xpd2.c.in
	$(ORACCIFY) xpd2.c.in
xpd2.c: xpd2.c.in
	$(ORACCIFY) xpd2.c.in

liboracc_la_SOURCES = \
  array.c     call.c	 dbi.c	     dra.c	file_io.c   fileutil.c \
  fname.c     getline.c	 pgetopt.c   hash.c	list.c	    messages.c \
  options.c   vector.c	 wildcard.c  xmem.c	xsystem.c   utf.c     \
  runexpat.c  loadfile.c xmlutil.c   pool.c	inctrie.c   npool.c \
  xmd.c       xpd2.c      memblock.c warning.c 	xrename.c   pct.c

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/otf/prj \
	-I@top_srcdir@/c/lib/expat/xmlparse
