## Process this file with automake to produce Makefile.in
##
#  Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
#  1997 under the GNU General Public License (see /psd/COPYING).
# 
#  $Id: Makefile.lam,v 0.5 1997/09/08 13:01:43 sjt Exp $

# sel24x
SUBDIRS = secatx setxtx secbdx setrax selemx 
@SET_MAKE@

.g.c:
	gperf -N `basename $*` -tT $< >$@

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/libstemmer_c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,lng} \
	-I/opt/local/include

AM_CFLAGS=-pedantic -ansi -Wall -g

AM_LDFLAGS=-L/opt/local/lib

bin_PROGRAMS=se ce masktest sedbg wmapper wmdebug wm ce_xtf ce2

ce_SOURCES=ce.c ce_xmd.c selib.c
ce_LDADD=@top_srcdir@/c/lib/liboracc.la

ce2_SOURCES=ce2.c ce_xmd2.c selib.c p2.c
ce2_LDADD=@top_srcdir@/c/lib/liboracc.la

ce_xtf_SOURCES=ce_xtf.c ce_xmd.c selib.c
ce_xtf_LDADD=@top_srcdir@/c/lib/liboracc.la

se_SOURCES=se.c setoks.c tokenize.c evaluate.c selib.c se_pcre.c binops.c \
	xkeys.c langmask.c propmask.c alias.c vid.c keymangler.c rulestab.c \
	xml.c binops24.c est.c

se_LDADD=@top_srcdir@/c/otf/chr/libchr.la \
	 @top_srcdir@/c/otf/lng/liblng.la \
	 @top_srcdir@/c/lib/liboracc.la \
	-L/lib64 -lpcre

sedbg_SOURCES = sedbg.c selib.c langmask.c mm_any.c rulestab.c vid.c
sedbg_LDADD =  @top_srcdir@/c/lib/liboracc.la

wm_SOURCES=wm.c wm_lib.c
wm_LDADD=@top_srcdir@/c/lib/liboracc.la

wmapper_SOURCES=wmapper.c wm_lib.c
wmapper_LDADD=@top_srcdir@/c/lib/liboracc.la

wmdebug_SOURCES=wmdebug.c wm_lib.c
wmdebug_LDADD=@top_srcdir@/c/lib/liboracc.la

EXTRA_DIST = *.h mapaccent.c xfuncs.c strndup.c
