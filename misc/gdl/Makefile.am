include ../../oracc-am-rules.txt

bin_SCRIPTS = gdlsort.plx
css_DATA = gdl.css
scriptdata_DATA = gdl-HTML.xsl gdl-ASL.xsl gdl-ATF.xsl gdl-chunk-renderer.xsl \
	gdl-render-g.xsl xgdlme.xsl gdl-TEST.xsl

gdl-HTML.xsl: gdl-HTML.xsl.in
	$(ORACCIFY) gdl-HTML.xsl.in

EXTRA_DIST = \
	gdl-ASL.xsl \
	gdl-ATF.xsl \
	gdl-HTML.xsl.in \
	gdl-chunk-renderer.xsl \
	gdl-render-g.xsl \
	gdl.css \
	gdlsort.plx \
	xgdlme.xsl
