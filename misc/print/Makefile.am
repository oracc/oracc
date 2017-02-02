include ../../oracc-am-rules.txt

bin_SCRIPTS = chresto-index.sh mk-cbd-index.sh cbd-json.plx index-json.plx
css_DATA = chresto-index.css gloss.css
scriptdata_DATA = chresto-index-TeX.xsl cbd-sort.xsl cbd-sortcodes.xsl cbd-sortkeys.xsl cbd-strip-cpd-refs.xsl chresto-index-HTML.xsl chresto-selector.xsl chresto-sort.xsl chresto-sortcodes.xsl chresto-sortkeys.xsl chresto-strip-cpd-refs.xsl index-HTML.xsl cbd-json.plx
data_DATA = names.xml printjob.xml
config_DATA = cbd-chresto.xcf

chresto-index.sh: chresto-index.sh.in
	$(ORACCIFY) chresto-index.sh.in

cbd-chresto.xcf: cbd-chresto.xcf.in
	$(ORACCIFY) cbd-chresto.xcf.in

chresto-index-TeX.xsl: chresto-index-TeX.xsl.in
	$(ORACCIFY) chresto-index-TeX.xsl.in

EXTRA_DIST = \
	cbd-chresto.xcf.in \
	cbd-sort.xsl \
	cbd-sortcodes.xsl \
	cbd-sortkeys.xsl \
	cbd-strip-cpd-refs.xsl \
	chresto-index-HTML.xsl \
	chresto-index-TeX.xsl.in \
	chresto-index.css \
	chresto-index.sh.in \
	chresto-selector.xsl \
	chresto-sort.xsl \
	chresto-sortcodes.xsl \
	chresto-sortkeys.xsl \
	chresto-strip-cpd-refs.xsl \
	gloss.css \
	index-HTML.xsl \
	mk-cbd-index.sh \
	names.xml \
	printjob.xml
