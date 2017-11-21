include ../../oracc-am-rules.txt

bin_SCRIPTS=p3-project-data.plx p3-project.sh p3-pager.plx p3-asrch.sh p3-colours.plx p3-collapse.plx \
	p3-selector.plx p3-transonly.plx p3-translangs.sh proxyhost.plx p3-wrapup.sh
css_DATA=p3.css p3cbd.css oas.css transonly_off.css transonly_on.css transnone.css p3css.xml p3cbdcss.xml \
	p3social.css
data_DATA=p3-template.xml as-base.xml p2colours.txt oas-template.xml oas-instance.xml pll_linkids.xml \
	p3colours.txt
js_DATA=p3.js oas.js p3-transonly.js
scriptdata_DATA=p3-project.xsl p3-ce-HTML.xsl p3-htmlview.xsl p3-xmd-div.xsl oas-SEARCH.xsl oas-RETURN.xsl \
	p3-html-wrapper.xsl p3-corpusview-lib.xsl p3-xtl-div.xsl p3-hub.xsl p3-social.xsl
html_DATA=noresults.div empty.div

oas-instance.xml: oas-instance.xml.in
	$(ORACCIFY) oas-instance.xml.in

p3-asrch.sh: p3-asrch.sh.in
	$(ORACCIFY) p3-asrch.sh.in

p3-ce-HTML.xsl: p3-ce-HTML.xsl.in
	$(ORACCIFY) p3-ce-HTML.xsl.in

p3css.xml: p3.css
	echo '<css>' >p3css.xml
	sed 's/\.css/css.xml/g' <p3.css >>p3css.xml
	echo '</css>' >>p3css.xml

p3cbdcss.xml: p3cbd.css
	echo '<css>' >p3cbdcss.xml
	cat p3cbd.css >>p3cbdcss.xml
	echo '</css>' >>p3cbdcss.xml

p3-colours.plx: p3-colours.plx.in
	$(ORACCIFY) p3-colours.plx.in

p3-hub.xsl: p3-hub.xsl.in
	$(ORACCIFY) p3-hub.xsl.in

p3-corpusview-lib.xsl: p3-corpusview-lib.xsl.in
	$(ORACCIFY) p3-corpusview-lib.xsl.in

p3-pager.plx: p3-pager.plx.in
	$(ORACCIFY) p3-pager.plx.in

p3-project.xsl: p3-project.xsl.in
	$(ORACCIFY) p3-project.xsl.in

p3-selector.plx: p3-selector.plx.in
	$(ORACCIFY) p3-selector.plx.in

p3-social.xsl: p3-social.xsl.in
	$(ORACCIFY) p3-social.xsl.in

p3-transonly.plx: p3-transonly.plx.in
	$(ORACCIFY) p3-transonly.plx.in

p3-xmd-div.xsl: p3-xmd-div.xsl.in
	$(ORACCIFY) p3-xmd-div.xsl.in

extra_DIST=\
	Slicer.pm
