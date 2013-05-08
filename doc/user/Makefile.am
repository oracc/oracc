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

install-data-local:
	$(HTMLINDEX) user $(DESTDIR)$(htmldir)/doc/user $(top_srcdir)

clean-local:
	rm index*.xml

EXTRA_DIST = order.txt
