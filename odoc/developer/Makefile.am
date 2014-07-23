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
	$(NO_ORDERXML)

all-local: index.xml

install-data-local:
	$(HTMLINDEX) developer $(DESTDIR)$(htmldir)/doc/developer $(top_srcdir)

clean-local:
	@rm -f index*.xml
