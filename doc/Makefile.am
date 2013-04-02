SUBDIRS = 00tools ns builder developer install manager user ood wwwhome urn sysadmin
html_DATA = index.html
EXTRA_DIST = index.html

install-data-local:
	ORACC=$(DESTDIR)$(prefix) @abs_top_builddir@/doc/00tools/estindex.sh $(DESTDIR)$(prefix)/www/doc
