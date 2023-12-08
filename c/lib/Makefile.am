## Process this file with automake to produce Makefile.in
##
#  Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
#  1997 under the GNU General Public License (see /psd/COPYING).
# 
#  $Id: Makefile.am,v 0.5 1997/09/08 13:02:15 sjt Exp $

AM_CFLAGS=-pedantic -Wall -g
SUBDIRS=expat libstemmer_c oracc rnv sas
noinst_LTLIBRARIES=liboracc.la
liboracc_la_LIBADD= \
	expat/libexpat.la \
	oracc/liboracc.la \
	libstemmer_c/libstemmer.la \
	rnv/librnv.la \
	sas/libsas.la

#	collate/libcoll.la 
#	skl/libskl.la
#	gsl/libgsl.la 
