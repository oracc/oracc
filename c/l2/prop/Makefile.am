include ../../../oracc-am-rules.txt

.g.c:
	gperf -N `basename $*` -tT $< >$@
SUBDIRS=

props_auto.c: props_auto.c.in
	$(ORACCIFY) props_auto.c.in

props_load.c: props_load.c.in
	$(ORACCIFY) props_load.c.in

instdir=$(libdir)/data
inst_DATA=props.xml props-qpn.xml

noinst_LTLIBRARIES=libprops.la
libprops_la_SOURCES=props_head.c props_link.c props_list.c \
	props_load.c props_util.c props_auto.c

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/otf/{atf,cdf} \
	-I@top_srcdir@/c/l2/{form,ilem,xcl2,link,ngram,psus,sigs,word} \
	-I@top_srcdir@/../oracc2/lib/{std,lng}

EXTRA_DIST=props.h *.xml
