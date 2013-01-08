include @top_srcdir@/oracc-doc-rules.txt

SUBDIRS = 

all-local:
	perl $(top_srcdir)/misc/xdf/xdfmanager.plx

clean-local:
	rm -fr html

install-data-local:
	$(top_srcdir)/oracc-doc-install.sh $(DESTDIR)$(prefix) $(top_srcdir)

EXTRA_DIST = \
	gdl.xdf \
	etc/1-n07a.png \
	etc/1-n07b.png \
	etc/4ban2-variant.png \
	etc/4ban2.png \
	etc/ash-curved.png \
	etc/ash-kabatenu.png \
	etc/ash-zidatenu.png \
	etc/ash.png \
	etc/colon.png \
	etc/diacolon.png \
	etc/du-gunu.png \
	etc/du-over-du.png \
	etc/du-sheshig.png \
	etc/du.png \
	etc/ga2-times-an.png \
	etc/ga2-times-me-en.png \
	etc/gan2-tenu.png \
	etc/gi-crossing-gi.png \
	etc/lu2-opposing-lu2.png \
	etc/lu2-times-4.png \
	etc/mul.png \
	etc/N01F.png \
	etc/naga180.png \
	etc/nigin2.png \
	etc/sag-nutillu.png \
	etc/sag.png \
	etc/tricolon.png \
	etc/u-vreflected.png \
	etc/worddiv.png
