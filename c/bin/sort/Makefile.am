bin_PROGRAMS=psdsort
psdsort_SOURCES=sort.c long-options.c xstrtod.c error.c \
	system.h long-options.h error.h xstrtod.h getopt1.c
psdsort_LDADD=@top_srcdir@/c/lib/liboracc.la @top_srcdir@/../oracc2/lib/liboracc2.la 
AM_CPPFLAGS =-I@top_srcdir@/c/include -I@top_srcdir@/../oracc2/lib/collate
