include @top_srcdir@/oracc-doc-rules.txt

SUBDIRS = AKK ARC ELX GRC PEO QPN SUX UGA

all-local:
	perl $(top_srcdir)/misc/xdf/xdfmanager.plx -prefix $(top_srcdir)

clean-local:
	rm -fr html

install-data-local:
	$(top_srcdir)/oracc-doc-install.sh $(DESTDIR)$(prefix) $(top_srcdir)

EXTRA_DIST = \
	linganno.xdf
