include ../../oracc-am-rules.txt

bin_SCRIPTS=p3-project-data.plx p3-project.sh p3-pager.plx p3-asrch.sh p3-colours.plx p3-collapse.plx \
	p3-selector.plx p3-transonly.plx p3-translangs.sh proxyhost.plx
css_DATA=p3.css p3cbd.css oas.css transonly_off.css transonly_on.css transnone.css
data_DATA=p3-template.xml as-base.xml p2colours.txt oas-template.xml oas-instance.xml
js_DATA=p3.js oas.js p3-transonly.js
scriptdata_DATA=p3-project.xsl p3-ce-HTML.xsl p3-htmlview.xsl p3-xmd-div.xsl oas-SEARCH.xsl oas-RETURN.xsl \
	p3-html-wrapper.xsl p3-corpusview-lib.xsl p3-xtl-div.xsl p3-hub.xsl
html_DATA=noresults.div empty.div

p3-asrch.sh: p3-asrch.sh.in
	$(ORACCIFY) p3-asrch.sh.in

p3-corpusview-lib.xsl: p3-corpusview-lib.xsl.in
	$(ORACCIFY) p3-corpusview-lib.xsl.in

p3-pager.plx: p3-pager.plx.in
	$(ORACCIFY) p3-pager.plx.in

p3-project.xsl: p3-project.xsl.in
	$(ORACCIFY) p3-project.xsl.in

p3-selector.plx: p3-selector.plx.in
	$(ORACCIFY) p3-selector.plx.in

p3-transonly.plx: p3-transonly.plx.in
	$(ORACCIFY) p3-transonly.plx.in

p3-xmd-div.xsl: p3-xmd-div.xsl.in
	$(ORACCIFY) p3-xmd-div.xsl.in

extra_DIST=\
	Slicer.pm
