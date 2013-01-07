include ../../oracc-doc-rules.txt

SUBDIRS = \
	cuneify \
	exist \
	fxr/1.0 \
	l2 \
	nsa \
	opendocument \
	projweb \
	psa \
	tei \
	uri \
	versions \
	xdftut \
	xmlarch

index.xml: 
	$(ORDERXML)

all-local: index.xml
	$(HTMLINDEX) developer $(PREFIX)$(htmldir)/doc/builder

clean-local:
	rm index*.xml
