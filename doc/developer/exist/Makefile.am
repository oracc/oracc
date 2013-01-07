include @top_srcdir@/oracc-doc-rules.txt

SUBDIRS = example1 example2 example3 example4 example5 example6 example7 example8 install projects starting

all-local:
	$(bindir)/xdfmanager.plx

clean-local:
	rm -fr html

install-data-local:
	$(top_srcdir)/oracc-doc-install.sh $(DESTDIR)$(prefix) $(top_srcdir)

EXTRA_DIST = \
	exist.xdf
