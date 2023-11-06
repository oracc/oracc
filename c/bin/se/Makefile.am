## Process this file with automake to produce Makefile.in
##
#  Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
#  1997 under the GNU General Public License (see /psd/COPYING).
# 
#  $Id: Makefile.lam,v 0.5 1997/09/08 13:01:43 sjt Exp $

include ../../../oracc-am-rules.txt

SUBDIRS = . secatx segdfx setxtx secbdx setrax selemx
@SET_MAKE@

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/libstemmer_c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,gdl,lib,run} \
	-I@top_srcdir@/../oracc2/lib/{include,std,sys,lng}
	-I/usr/local/include

AM_LDFLAGS=-L/opt/local/lib

bin_PROGRAMS=se masktest sedbg wmapper wmdebug wm ce_xtf ce2

#v2
#v2_SOURCES=v2.c
#v2_LDADD=@top_srcdir@/c/lib/liboracc.la

ce2_SOURCES=ce2.c ce_xmd2.c selib.c p2.c ce_gdf.c v2.c
ce2_LDADD=@top_srcdir@/c/lib/liboracc.la

ce_xtf_SOURCES=ce_xtf.c ce_xmd.c selib.c v2.c
ce_xtf_LDADD=@top_srcdir@/c/lib/liboracc.la

se_SOURCES=se.c setoks.c tokenize.c evaluate.c selib.c se_pcre.c binops.c \
	xkeys.c langmask.c propmask.c alias.c vid.c keymangler.c rulestab.c \
	xml.c binops24.c est.c signmap.c v2.c

se_LDADD=@top_srcdir@/c/otf/chr/libchr.la \
	 @top_srcdir@/c/lib/liboracc.la \
	-L/lib64 -lpcre @top_srcdir@/../oracc2/lib/liboracc2.la

sedbg_SOURCES = sedbg.c selib.c langmask.c mm_any.c rulestab.c vid.c v2.c
sedbg_LDADD =  @top_srcdir@/c/lib/liboracc.la

wm_SOURCES=wm.c wm_lib.c
wm_LDADD=@top_srcdir@/c/lib/liboracc.la

wmapper_SOURCES=wmapper.c wm_lib.c
wmapper_LDADD=@top_srcdir@/c/lib/liboracc.la

wmdebug_SOURCES=wmdebug.c wm_lib.c
wmdebug_LDADD=@top_srcdir@/c/lib/liboracc.la

EXTRA_DIST = *.h mapaccent.c xfuncs.c strndup.c

ce.c: ce.c.in
	$(ORACCIFY) ce.c.in

ce2.c: ce2.c.in
	$(ORACCIFY) ce2.c.in

ce_xmd.c: ce_xmd.c.in
	$(ORACCIFY) ce_xmd.c.in

signmap.c: signmap.c.in
	$(ORACCIFY) signmap.c.in

