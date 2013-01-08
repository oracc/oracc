include @top_srcdir@/oracc-doc-rules.txt

SUBDIRS = esp espbuild espcontent espelements esperrors espimages espsetup projurls

all-local:
	perl $(top_srcdir)/misc/xdf/xdfmanager.plx

clean-local:
	rm -fr html

install-data-local:
	$(top_srcdir)/oracc-doc-install.sh $(DESTDIR)$(prefix) $(top_srcdir)

EXTRA_DIST = \
	portals.xdf
