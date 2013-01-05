include ../../oracc-am-rules.txt

bin_SCRIPTS = highchars.plx ltype.plx vg.sh xl
scriptdata_DATA = findfile.xsl test-findfile.xsl

findfile.xsl: findfile.xsl.in
	$(ORACCIFY) findfile.xsl.in

EXTRA_DIST = \
	findfile.xsl.in \
	highchars.plx \
	ltype.plx \
	test-findfile.xsl \
	vg.sh \
	xl
