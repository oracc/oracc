VERSION=1.7.6sjt
EXPAT_H="<expat.h>"
UNISTD_H="<unistd.h>"
SCM_H="<scm/scm.h>"

# optional features
M_STATIC=0
M_FILL=0
DSL_SCM=0
DXL_EXC=0

DEF=\
-DM_STATIC=${M_STATIC} \
-DM_FILL=${M_FILL} \
-DEXPAT_H=${EXPAT_H} \
-DUNISTD_H=${UNISTD_H} \
-DRNV_VERSION="\"${VERSION}\"" \
-DARX_VERSION="\"${VERSION}\"" \
-DRVP_VERSION="\"${VERSION}\""
WARN=-Wall -Wstrict-prototypes  -Wmissing-prototypes -Wcast-align
OPT=-O -g

AM_CFLAGS=${INC} ${DEF} ${WARN} ${OPT}

noinst_LTLIBRARIES = librnv.la
librnv_la_SOURCES = \
	ary.h ary.c \
	drv.h drv.c \
	dsl.h dsl.c \
	dxl.h dxl.c \
	er.h er.c \
	erbit.h \
	ht.h ht.c \
	ll.h  \
	m.h m.c \
	rn.h rn.c \
	rnc.h rnc.c \
	rnd.h rnd.c \
	rnl.h rnl.c \
	rnv.h rnv.c \
	rnx.h rnx.c \
	rx.h rx.c \
	s.h s.c \
	sc.h sc.c \
	u.h u.c \
	xmlc.h xmlc.c \
	xsd.h xsd.c \
	xsd_tm.h xsd_tm.c

EXTRA_DIST=rx_cls_ranges.c rx_cls_u.c

#bin_PROGRAMS=rnv
#rnv_SOURCES=rnv.c
#rnv_LDADD=librnv.la -lexpat
