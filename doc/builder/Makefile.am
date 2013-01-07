include ../../oracc-doc-rules.txt

SUBDIRS = \
        bilinguals \
        cdli \
        dates \
        emacs \
        l2 \
        labels \
        layouts \
        linganno \
        math \
        metrology \
        numbers \
        primer \
        quickref \
        scores \
        translation

index.xml: order.txt
	$(ORDERXML)

all-local: index.xml
	$(HTMLINDEX) builder $(DESTDIR)$(htmldir)/doc/builder

clean-local:
	rm index*.xml

EXTRA_DIST = order.txt atfpages.txt
