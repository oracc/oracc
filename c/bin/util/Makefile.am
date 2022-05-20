## Process this file with automake to produce Makefile.in
##
#  Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
#  1997 under the GNU General Public License (see /psd/COPYING).
# 
#  $Id: Makefile.lam,v 0.5 1997/09/08 13:01:43 sjt Exp $

include ../../../oracc-am-rules.txt

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,gdl,lib,lng} \
	-I@top_srcdir@/c/lib/{collate,sas} \
	-I@top_srcdir@/c/l2/{form,ilem,link,ngram,prop,psus,sigs,word,xcl2}

AM_CFLAGS=-pedantic -Wall

bin_PROGRAMS=concord pqxpand haslinks haslinks1 isogmt \
	jpegsize linkfiles gdf labeltable unittable \
	newext oraccopt pqunion sigfixer sigmap signiary xmlnl \
	xmlns xmlsplit utf8v xalias xfrag filesize listdir \
	xns xisdb wid2err rootnode wid2lem xis2json linksets \
	xtfsigs

#pqoutline pqsort xml2json

xis2json_SOURCES=xis2json.c
xis2json_LDADD=@top_srcdir@/c/lib/liboracc.la

haslinks.c: haslinks.c.in
	$(ORACCIFY) haslinks.c.in

pqoutline.c: pqoutline.c.in
	$(ORACCIFY) pqoutline.c.in

sigmap.c: sigmap.c.in
	$(ORACCIFY) sigmap.c.in

concord_SOURCES=concord.c
concord_LDADD=@top_srcdir@/c/lib/liboracc.la

gdf_LDADD=@top_srcdir@/c/lib/liboracc.la

haslinks_SOURCES=haslinks.c

isogmt_SOURCES=isogmt.c

labeltable_LDADD=@top_srcdir@/c/lib/liboracc.la

linkfiles_SOURCES=linkfiles.c

jpegsize_SOURCES=jpegsize.c jpeginfo.c jpeginfo.h

newext_SOURCES=newext.c
newext_LDADD=@top_srcdir@/c/lib/liboracc.la

oraccopt_SOURCES=oraccopt.c
oraccopt_LDADD = @top_srcdir@/c/lib/liboracc.la

#pqoutline_SOURCES=pqoutline.c otlenum.h
#pqoutline_LDADD = ../expat/libexpat.la @top_srcdir@/c/lib/liboracc.la

#pqsort_SOURCES=pqsort.c
#pqsort_LDADD = @top_srcdir@/c/lib/liboracc.la

pqunion_SOURCES=pqunion.c

pqxpand_SOURCES=pqxpand.c
pqxpand_LDADD = @top_srcdir@/c/lib/liboracc.la

signiary_SOURCES=signiary.c
signiary_LDADD = @top_srcdir@/c/lib/liboracc.la

sigfixer_SOURCES=sigfixer.c
sigfixer_LDADD = @top_srcdir@/c/lib/liboracc.la

sigmap_SOURCES=sigmap.c
sigmap_LDADD = @top_srcdir@/c/lib/liboracc.la

unittable_LDADD=@top_srcdir@/c/lib/liboracc.la

xalias_SOURCES=xalias.c
xalias_LDADD = @top_srcdir@/c/l2/libl2.la \
	@top_srcdir@/c/otf/gdl/libgdl.la \
	@top_srcdir@/c/lib/liboracc.la

rootnode_LDADD=@top_srcdir@/c/lib/liboracc.la

xfrag_LDADD=@top_srcdir@/c/lib/liboracc.la

xisdb_SOURCES=xisdb.c
xisdb_LDADD = @top_srcdir@/c/lib/liboracc.la

xns_SOURCES=xns.c
xns_LDADD = @top_srcdir@/c/lib/liboracc.la

xmlns_SOURCES=xmlns.c
xmlns_LDADD = @top_srcdir@/c/lib/liboracc.la

xmlsplit_SOURCES=xmlsplit.c
xmlsplit_LDADD = @top_srcdir@/c/lib/liboracc.la

utf8v_sources=utf8v.c

linksets_LDADD=@top_srcdir@/c/lib/liboracc.la

wid2err_LDADD=@top_srcdir@/c/lib/liboracc.la

wid2lem_LDADD=@top_srcdir@/c/lib/liboracc.la

xtfsigs_LDADD=@top_srcdir@/c/lib/liboracc.la

