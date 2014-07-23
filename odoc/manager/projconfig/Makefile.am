include @top_srcdir@/oracc-doc-rules.txt

SUBDIRS = c2

all-local:
	perl $(top_srcdir)/misc/xdf/xdfmanager.plx -prefix $(top_srcdir)

clean-local:
	rm -fr html

install-data-local:
	$(top_srcdir)/oracc-doc-install.sh $(DESTDIR)$(prefix) $(top_srcdir)

EXTRA_DIST = \
	projconfig.xdf
