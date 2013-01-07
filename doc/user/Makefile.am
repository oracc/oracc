include ../../oracc-doc-rules.txt

SUBDIRS = \
	advanced \
	citing \
	fonts \
	fulltextsearch \
	glossaries \
	keyboards \
	p2 \
	searching \
	unicode \
	userguide

#	projurls

index.xml: order.txt
	$(ORDERXML)

all-local: index.xml
	$(HTMLINDEX) user $(DESTDIR)$(htmldir)/doc/user

clean-local:
	rm index*.xml

EXTRA_DIST = order.txt
