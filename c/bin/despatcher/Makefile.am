include ../../../oracc-am-rules.txt

subdirs=ccgi

.g.c:
	gperf -N `basename $*` -tT $< >$@

AM_CPPFLAGS=-I@top_srcdir@/c/include -Wall -pedantic -ansi

cgibin_PROGRAMS=oracc-resolver

oracc_resolver_SOURCES=oracc-resolver.c or_builtins.c or_stable_uris.c \
	selections.c builtins.c or_response.c or_common.c \
	legacy.c or_gdf.c or_pqx.c pqx_map.c pqx_h_tab.c ncname.c p3.c

oracc_resolver_LDADD=ccgi/libccgi.a

ccgi/libccgi.a:
	(cd ccgi ; make)

or_builtins.c: or_builtins.c.in
	$(ORACCIFY) or_builtins.c.in

or_common.c: or_common.c.in
	$(ORACCIFY) or_common.c.in

or_gdf.c: or_gdf.c.in
	$(ORACCIFY) or_gdf.c.in

or_pqx.c: or_pqx.c.in
	$(ORACCIFY) or_pqx.c.in

oracc-resolver.c: oracc-resolver.c.in ccgi/libccgi.a
	$(ORACCIFY) oracc-resolver.c.in

or_response.c: or_response.c.in
	$(ORACCIFY) or_response.c.in

or_stable_uris.c: or_stable_uris.c.in
	$(ORACCIFY) or_stable_uris.c.in

or_xforms.c: or_xforms.c.in
	$(ORACCIFY) or_xforms.c.in

pqx_map.c: pqx_map.c.in
	$(ORACCIFY) pqx_map.c.in

EXTRA_DIST=*.h
