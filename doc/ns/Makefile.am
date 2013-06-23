include ../../oracc-doc-rules.txt

SUBDIRS = \
	xtr/1.0 \
	cbd/1.0 \
	cbd/2.0 \
	cdf/1.0 \
	esp/1.0 \
	gdl/1.0 \
	sl/1.0 \
	xcf/1.0 \
	xcl/1.0 \
	xdf/1.0 \
	xmd/1.0 \
	xpd/1.0 \
	xse/1.0 \
	xst/1.0 \
	xtc/1.0 \
	xtf/1.0

#	xsf/1.0 

index.xml: 
	$(NO_ORDERXML)

all-local: index.xml

install-data-local:
	$(HTMLINDEX) ns $(DESTDIR)$(htmldir)/doc/ns $(top_srcdir)
	rm -f $(DESTDIR)$(htmldir)/ns
	ln -sf $(DESTDIR)$(htmldir)/doc/ns $(DESTDIR)$(htmldir)/ns

clean-local:
	@rm -f index*.xml
