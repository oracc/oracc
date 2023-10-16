AM_CPPFLAGS=-I../include

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
