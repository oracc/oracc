include ../../oracc-am-rules.txt

bin_SCRIPTS = chresto-index.sh mk-cbd-index.sh bki-HTML.sh \
	r-labels.plx r-sort.plx r-uniq.plx r-trim.plx \
	mk-labels.sh mk-labels.plx r-trim-label2.plx r-vol.plx \
	bki-split-qpn.plx bki-merge-xrr.sh bki-ixstyle-rinap.sh \
	bki-names-tab.sh bki-names-xml.sh

css_DATA = chresto-index.css gloss.css

scriptdata_DATA = chresto-index-TeX.xsl cbd-sort.xsl			\
	cbd-sortcodes.xsl cbd-sortkeys.xsl cbd-strip-cpd-refs.xsl	\
	chresto-index-HTML.xsl chresto-selector.xsl chresto-sort.xsl	\
	chresto-sortcodes.xsl chresto-sortkeys.xsl			\
	chresto-strip-cpd-refs.xsl index-HTML.xsl 			\
	bki-subset.xsl bki-xis.xsl bki-xis-group.xsl bki-labels.xsl bki-sort.xsl \
	bki-akk-HTML.xsl bki-qpn-HTML.xsl bki-sux-HTML.xsl mk-labels.xsl \
	bki-chresto-HTML.xsl bki-flatten-rr.xsl bki-group-rr.xsl \
	bki-names-tab.xsl

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
