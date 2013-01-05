include @top_srcdir@/oracc-doc-rules.txt


all-local:
	$(bindir)/xdfmanager.plx

clean-local:
	rm -fr html

install-data-local:
	$(top_srcdir)/oracc-doc-install.sh $(DESTDIR)$(prefix) $(top_srcdir)

EXTRA_DIST = \
	math.xdf \
	etc/cunei-0c.png \
	etc/cunei-20.png \
	etc/cunei-40.png \
	etc/cunei-50.png \
	etc/cunei-60.png \
	etc/cunei-70.png \
	etc/cunei-80.png \
	etc/cunei-90.png
