include ../../oracc-am-rules.txt

bin_SCRIPTS = l2p3.sh
scriptdata_DATA = g2-gdl-HTML.xsl g2-gdl-TeX.xsl g2-xtf-HTML.xsl cbd-HTML-OBF.xsl \
	g2-base-s.xsl g2-chunk-html.xsl g2-gdl-render-g.xsl g2-HTML-articles.xsl \
	g2-HTML-summaries.xsl g2-overview.xsl g2-primary-base.xsl g2-sig.xsl \
	g2-split-html.xsl g2-summaries.xsl g2-TeX-articles.xsl g2c-sig-map.xsl \
	p2-summaries-HTML.xsl p2-summaries.xsl g2-xsf-HTML.xsl

config_DATA = l2p3.xcf

g2-xsf-HTML.xsl: g2-xsf-HTML.xsl.in
	$(ORACCIFY) g2-xsf-HTML.xsl.in

g2-xtf-HTML.xsl: g2-xtf-HTML.xsl.in
	$(ORACCIFY) g2-xtf-HTML.xsl.in

g2-gdl-HTML.xsl: g2-gdl-HTML.xsl.in
	$(ORACCIFY) g2-gdl-HTML.xsl.in

g2-gdl-TeX.xsl: g2-gdl-TeX.xsl.in
	$(ORACCIFY) g2-gdl-TeX.xsl.in

EXTRA_DIST = \
	cbd-HTML-OBF.xsl \
	g2-HTML-articles.xsl \
	g2-HTML-summaries.xsl \
	g2-TeX-articles.xsl \
	g2-base-s.xsl \
	g2-chunk-html.xsl \
	g2-gdl-HTML.xsl.in \
	g2-gdl-TeX.xsl.in \
	g2-gdl-render-g.xsl \
	g2-overview.xsl \
	g2-primary-base.xsl \
	g2-sig.xsl \
	g2-split-html.xsl \
	g2-summaries.xsl \
	g2-xsf-HTML.xsl.in \
	g2-xtf-HTML.xsl.in \
	g2c-sig-map.xsl \
	l2p3.sh \
	l2p3.xcf \
	p2-summaries-HTML.xsl \
	p2-summaries.xsl
