include ../../oracc-am-rules.txt

bin_SCRIPTS = test-ce.plx
scriptdata_DATA = ce-labels.xsl ce-render.xsl ce-xtf-HTML.xsl s2-ce-xtf-HTML.xsl

ce-labels.xsl: ce-labels.xsl.in
	$(ORACCIFY) ce-labels.xsl.in

EXTRA_DIST = \
	CE.pm \
	ce-labels.xsl.in \
	ce-render.xsl \
	ce-xtf-HTML.xsl \
	s2-ce-xtf-HTML.xsl \
	test-ce.plx
