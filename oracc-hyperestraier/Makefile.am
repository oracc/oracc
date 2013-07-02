include ../oracc-am-rules.txt

MYCONFS = estseek.conf estseek.tmpl estseek.top estseek.help \
  estfraud.conf estproxy.conf estscout.conf estsupt.conf \
  estresult.dtd estraier.idl

MYHTML = *.html

cgidata_DATA = $(MYCONFS)
cgibin_PROGRAMS=estseek
html_DATA = $(MYHTML)
bin_PROGRAMS=estcmd 
# estmttest estmaster estbutler estcall estwaver estload
bin_SCRIPTS=dbi2est.plx estwolefind

# estconfig

noinst_LTLIBRARIES=libestraier.la

libestraier_la_SOURCES = estraier.c estmtdb.c estnode.c md5.c

x_LDADD = -lestraier -lqdbm -lm -lz

AM_CPPFLAGS = -I/usr/local/include -I/usr/include/qdbm @QDBMINCLUDE@

AM_CFLAGS = -fPIC @QDBMLIBDIR@

MYHEADS = estraier.h estmtdb.h estnode.h

MYPCS = hyperestraier.pc

estseek_SOURCES = estseek.c

estseek_LDADD = $(x_LDADD)

estcmd_SOURCES = estcmd.c mymorph.c

estcmd_LDADD = $(x_LDADD)

EXTRA_DIST=*.h $(MYCONFS) $(MYHTML)
