include @top_srcdir@/oracc-doc-rules.txt

SUBDIRS = \
	PSUs \
	BFFs \
	F2 \
	howtouseit \
	glossaries \
	lem \
	COFs \
	lemsyntax \
	howitworks

index.xml:
	$(NO_ORDERXML)

all-local: index.xml
	$(HTMLINDEX) builder/l2 $(DESTDIR)$(htmldir)/doc/builder/l2

