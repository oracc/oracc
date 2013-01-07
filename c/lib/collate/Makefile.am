## Process this file with automake to produce Makefile.in
##
#  Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
#  1997 under the GNU General Public License (see /psd/COPYING).
# 
#  $Id: Makefile.am,v 0.5 1997/09/08 13:01:43 sjt Exp $
INCLUDES=-I@top_srcdir@/c/include

COLLSRCS=unicode.c

noinst_SCRIPTS=seq2c mktablst

noinst_LTLIBRARIES=libcoll.la
libcoll_la_SOURCES=$(COLLSRCS) tablist.c collate.c

DISTCLEANFILES = $(COLLSRCS) tablist.c

EXTRA_DIST=unicode.seq colltype.h collate.h

tablist.c: mktablst unicode.c
	perl ./mktablst unicode.c

unicode.c: seq2c unicode.seq iso10646
	perl ./seq2c $(srcdir)/unicode.seq
