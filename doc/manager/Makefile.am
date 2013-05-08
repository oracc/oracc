include ../../oracc-doc-rules.txt

SUBDIRS = \
	filesystem \
	portals \
	projcats \
	projconfig \
	projects \
	projemacs \
	projlife \
	projunix \
	proxies \
	the-oracc-command \
	webcolours

#	atftools 

index.xml: order.txt
	$(ORDERXML)

all-local: index.xml

install-data-local:
	$(HTMLINDEX) manager $(DESTDIR)$(htmldir)/doc/manager $(top_srcdir)

clean-local:
	rm index*.xml

EXTRA_DIST = order.txt atfpages.txt
