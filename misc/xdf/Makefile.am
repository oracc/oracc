include ../../oracc-am-rules.txt

bin_SCRIPTS = rnc2rng.sh xdfmanager.plx podhtml-filter.plx xdftemplate.plx xmlformat.plx
css_DATA = colorer.css
scriptdata_DATA = xdf-HTML.xsl
data_DATA = refs.xml

rnc2rng.sh: rnc2rng.sh.in
	$(ORACCIFY) rnc2rng.sh.in

xdfmanager.plx: xdfmanager.plx.in
	$(ORACCIFY) xdfmanager.plx.in

EXTRA_DIST = \
	colorer.css \
	podhtml-filter.plx \
	refs.xml \
	rnc2rng.sh.in \
	xdf-HTML.xsl \
	xdfmanager.plx.in \
	xdftemplate.plx \
	xmlformat.plx
