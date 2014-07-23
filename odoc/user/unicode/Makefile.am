include @top_srcdir@/oracc-doc-rules.txt
ORACCIFY = $(abs_top_builddir)/oracc-subst.sh $(DESTDIR)$(prefix)

SUBDIRS = 

unitab.plx: unitab.plx.in
	$(ORACCIFY) unitab.plx.in

all-local:
	perl $(top_srcdir)/misc/xdf/xdfmanager.plx -prefix $(top_srcdir)

clean-local:
	rm -fr html

install-data-local:
	$(top_srcdir)/oracc-doc-install.sh $(DESTDIR)$(prefix) $(top_srcdir)

EXTRA_DIST = \
	unicode.xdf \
	unitab.plx.in
