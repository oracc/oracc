## Process this file with automake to produce Makefile.in
##
#  Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
#  1997 under the GNU General Public License (see /psd/COPYING).
# 
#  $Id: Makefile.am,v 0.5 1997/09/08 13:01:43 sjt Exp $

include ../../../oracc-am-rules.txt

AM_CPPFLAGS=-I@top_srcdir@/c/include -I@top_srcdir@/c/lib/expat/xmlparse -I/opt/local/include


AM_LDFLAGS=-L/opt/local/lib

bin_PROGRAMS=pgcsi pgu showcsi ptoqtest pd pg2 xisperiods

#pg 

pd.c: pd.c.in
	$(ORACCIFY) pd.c.in

#pg.c: pg.c.in
#	$(ORACCIFY) pg.c.in

pg2.c: pg2.c.in
	$(ORACCIFY) pg2.c.in

ptoq.c: ptoq.c.in
	$(ORACCIFY) ptoq.c.in

pgcsi_SOURCES=pgcsi.c sortinfo.c
pgcsi_LDADD = @top_srcdir@/c/lib/liboracc.la

#pg_SOURCES=pg.c sortinfo.c sort.c redblack.c sicache.c \
#	debug.c page.c outline.c ptoq.c
#pg_LDADD = -L/opt/local/lib @top_srcdir@/c/lib/liboracc.la -lgdbm

pg2_SOURCES=pg2.c sortinfo.c sort.c redblack.c sicache.c \
	debug.c page.c outline.c ptoq.c p2.c
pg2_LDADD = @top_srcdir@/c/lib/liboracc.la -lgdbm

pd_SOURCES=pd.c sortinfo.c sort.c redblack.c sicache.c \
	debug.c
pd_LDADD = @top_srcdir@/c/lib/liboracc.la -lgdbm

pgu_SOURCES=pgu.c

ptoqtest_SOURCES=ptoqtest.c ptoq.c
ptoqtest_LDADD=@top_srcdir@/c/lib/liboracc.la -lgdbm

showcsi_SOURCES=showcsi.c sortinfo.c

xisperiods_SOURCES=xisperiods.c sortinfo.c sort.c redblack.c sicache.c \
	debug.c periodmap.c

xisperiods_LDADD = @top_srcdir@/c/lib/liboracc.la -lgdbm

EXTRA_DIST=*.h sk_lookup.c pg_load.c

sortinfo.c: sortinfo.c.in
	$(ORACCIFY) sortinfo.c.in
