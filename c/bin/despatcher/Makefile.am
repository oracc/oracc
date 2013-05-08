include ../../../oracc-am-rules.txt
.g.c:
	gperf -N `basename $*` -tT $< >$@

AM_CPPFLAGS=-I@top_srcdir@/c/include

noinst_PROGRAMS=oracc-resolver

oracc_resolver_SOURCES=oracc-resolver.c or_builtins.c or_stable_uris.c \
	selections.c builtins.c or_response.c or_session.c or_common.c \
	legacy.c or_gdf.c or_pqx.c pqx_map.c pqx_h_tab.c ncname.c

install-exec-local:
	./cgi-install.sh ${ORACC_VAR}/www/cgi-bin

EXTRA_DIST=*.h

or_builtins.c: or_builtins.c.in
	$(ORACCIFY) or_builtins.c.in

or_common.c: or_common.c.in
	$(ORACCIFY) or_common.c.in

or_response.c: or_response.c.in
	$(ORACCIFY) or_response.c.in

or_stable_uris.c: or_stable_uris.c.in
	$(ORACCIFY) or_stable_uris.c.in

oracc-resolver.c: oracc-resolver.c.in
	$(ORACCIFY) oracc-resolver.c.in
