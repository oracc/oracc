include ../../oracc-am-rules.txt

bin_SCRIPTS=p3-project-data.plx p3-project.sh p3-pager.plx
css_DATA=p3.css p3cbd.css
data_DATA=p3-template.xml
js_DATA=p3.js
scriptdata_DATA=p3-project.xsl p3-ce-HTML.xsl p3-htmlview.xsl p3-xmd-div.xsl

p3-pager.plx: p3-pager.plx.in
	$(ORACCIFY) p3-pager.plx.in

p3-xmd-div.xsl: p3-xmd-div.xsl.in
	$(ORACCIFY) p3-xmd-div.xsl.in

extra_DIST=\
	Slicer.pm
