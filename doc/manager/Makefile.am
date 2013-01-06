include ../../oracc-doc-rules.txt

SUBDIRS = \
	atftools \
	filesystem \
	index.xml \
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

index.xml: order.txt
	$(ORDERXML)

all-local: index.xml
	$(HTMLINDEX) builder @htmldir@/doc/builder

clean-local:
	rm index*.xml

EXTRA_DIST = order.txt atfpages.txt
