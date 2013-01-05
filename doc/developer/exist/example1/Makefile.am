include @top_srcdir@/oracc-doc-rules.txt


all-local:
	$(bindir)/xdfmanager.plx

clean-local:
	rm -fr html

install-data-local:
	$(top_srcdir)/oracc-doc-install.sh $(DESTDIR)$(prefix) $(top_srcdir)

EXTRA_DIST = \
	example1.xdf \
	etc/stats-xql.css \
	etc/stats-xql.png \
	etc/stats.xql
