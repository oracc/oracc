include @top_srcdir@/oracc-doc-rules.txt

SUBDIRS = 

all-local:
	$(bindir)/xdfmanager.plx

clean-local:
	rm -fr html

install-data-local:
	$(top_srcdir)/oracc-doc-install.sh $(DESTDIR)$(prefix) $(top_srcdir)

EXTRA_DIST = \
	emacsw32.xdf
