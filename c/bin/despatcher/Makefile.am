.g.c:
	gperf -N `basename $*` -tT $< >$@

noinst_PROGRAMS=oracc-resolver

oracc_resolver_SOURCES=oracc-resolver.c or_builtins.c or_stable_uris.c \
	selections.c builtins.c or_response.c or_session.c or_common.c \
	legacy.c or_gdf.c or_pqx.c pqx_map.c pqx_h_tab.c ncname.c

#install:
#	./install.sh ${ORACC_VAR}/www/cgi-bin

EXTRA_DIST=*.h
