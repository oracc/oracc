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

bin_PROGRAMS=rnv
rnv_SOURCES=xcl.c
rnv_LDADD=@top_srcdir@/c/lib/liboracc.la -lexpat
AM_CPPFLAGS=-I@top_srcdir@/c/lib/rnv
