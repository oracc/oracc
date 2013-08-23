## Process this file with automake to produce Makefile.in
##
#  Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
#  1997 under the GNU General Public License (see /psd/COPYING).
# 
#  $Id: Makefile.am,v 0.5 1997/09/08 13:01:43 sjt Exp $

AM_CFLAGS=-ansi

AM_CPPFLAGS=-I@top_srcdir@/c/include -I@top_srcdir@/c/lib/collate \
	 -I@top_srcdir@/c/otf/{chr,gdl,lib,lng} \
	 -I@top_srcdir@/c/otf/{otf,ox}

#-I@top_srcdir@/c/l2/xcl2 \
#	 -I@top_srcdir@/c/l2/ilem  -I@top_srcdir@/c/l2/links

bin_PROGRAMS=matrix
matrix_SOURCES= \
	iterate.c   pf-clear.c  pf-sf4.c    pf-vars.c   sources.c \
	matrix.c    pf-cmp.c    pf-skel.c   pf_lib.c    tabloc.c \
	pf-align.c  pf-debug.c  pf-tex.c    sort.c      variants.c \
        expand.c    pf-xpand.c  phantom.c   pf-mtx.c    gdlif.c \
	matrix.h    pf_lib.h    sources.h   variants.h \
	@top_srcdir@/c/otf/ox/globals.c

matrix_LDADD=@top_srcdir@/c/lib/collate/libcoll.la \
	@top_srcdir@/c/otf/prj/libprj.la \
	@top_srcdir@/c/otf/gdl/libgdl.la \
	@top_srcdir@/c/otf/atf/libatf.la \
	@top_srcdir@/c/otf/lib/libcdf.la \
	@top_srcdir@/c/otf/chr/libchr.la \
	@top_srcdir@/c/otf/lng/liblng.la \
	@top_srcdir@/c/l2/libl2.la \
	@top_srcdir@/c/lib/liboracc.la
