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
	$(HTMLINDEX) manager $(PREFIX)$(htmldir)/doc/manager

clean-local:
	rm index*.xml

EXTRA_DIST = order.txt atfpages.txt
