## Process this file with automake to produce Makefile.in
##
#  Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
#  1997 under the GNU General Public License (see /psd/COPYING).
# 
#  $Id: Makefile.am,v 0.5 1997/09/08 13:02:15 sjt Exp $

AM_CFLAGS=-ansi -pedantic -Wall
SUBDIRS=
noinst_LTLIBRARIES=libskl.la
# noinst_PROGRAMS=sklfind sklshow
libskl_la_SOURCES = skl_copy.c skl_file.c skl_load.c sky_find.c smp_load.c

#sklfind_SOURCES=sklfind.c
#sklfind_LDADD=./libskl.la ../lib/liboracc.la
#
#sklshow_SOURCES=sklshow.c
#sklshow_LDADD=./libskl.la ../lib/liboracc.la

INCLUDES=-I@top_srcdir@/c/include

