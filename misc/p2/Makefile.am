include ../../oracc-am-rules.txt

htmlp2dir       = $(prefix)/www/p2

js_DATA = oas.js p2-keys.js p2-shortcuts.js p2.js p2-p1.js
scriptdata_DATA = p2-corpusview-lib.xsl p2-corpusview.xsl bad-include-p2-htmlview.xsl default-instance.xsl oas-RETURN.xsl oas-SEARCH.xsl p2-cbd-toc.xsl p2-corpusview-lib-wrapper.xsl p2-htmlview.xsl p2-project.xsl p2-xmd-div.xsl p2-xmd.xsl p2-xtl-div.xsl
data_DATA = as-base.xml p2-base.xml p2colours.txt pll_linkids.xml schemas.xml \
	default-browse.xml default-pager.xml default-search.xml
bin_SCRIPTS = p2-pager.plx install.sh p2-browse.plx p2-project-data.plx p2-project.sh p2-search.plx p2-sig.plx sessionify
css_DATA = oas.css p2-p1.css p2.css
htmlp2_DATA = p2-cbd-init.html p2-content.html p2-outline.html

p2-pager.plx: p2-pager.plx.in
	$(ORACCIFY) p2-pager.plx.in

p2-corpusview.xsl: p2-corpusview.xsl.in
	$(ORACCIFY) p2-corpusview.xsl.in

p2-corpusview-lib.xsl: p2-corpusview-lib.xsl.in
	$(ORACCIFY) p2-corpusview-lib.xsl.in

default-browse.xml: default-instance.xsl
	xsltproc -stringparam instance browse default-instance.xsl p2-base.xml >default-browse.xml

default-pager.xml: default-instance.xsl
	xsltproc -stringparam instance pager default-instance.xsl p2-base.xml >default-pager.xml

default-search.xml: default-instance.xsl
	xsltproc -stringparam instance search default-instance.xsl p2-base.xml >default-search.xml

EXTRA_DIST = \
	Util2.pm \
	as-base.xml \
	bad-include-p2-htmlview.xsl \
	default-instance.xsl \
	install.sh \
	oas-RETURN.xsl \
	oas-SEARCH.xsl \
	oas.css \
	oas.js \
	p2-base.xml \
	p2-browse.plx \
	p2-cbd-init.html \
	p2-cbd-toc.xsl \
	p2-content.html \
	p2-corpusview-lib-wrapper.xsl \
	p2-corpusview-lib.xsl.in \
	p2-corpusview.xsl.in \
	p2-htmlview.xsl \
	p2-keys.js \
	p2-outline.html \
	p2-p1.css \
	p2-pager.plx.in \
	p2-project-data.plx \
	p2-project.sh \
	p2-project.xsl \
	p2-search.plx \
	p2-shortcuts.js \
	p2-sig.plx \
	p2-xmd-div.xsl \
	p2-xmd.xsl \
	p2-xtl-div.xsl \
	p2.css \
	p2.js \
	p2colours.txt \
	pll_linkids.xml \
	schemas.xml \
	sessionify
