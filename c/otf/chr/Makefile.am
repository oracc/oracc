AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libchr.la

libchr_la_SOURCES = utf.c wcs.c atf.c utf8check.c charsets.c \
	subdig.c accnum.c sexify.c

# atf2utf.c g_c10e.c

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/otf/lib \
	-I@top_srcdir@/../oracc2/lib/{std,lng}

charsets.c: charsets.txt charsets.plx
	perl ./charsets.plx >charsets.c
