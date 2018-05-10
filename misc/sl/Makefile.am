include ../../oracc-am-rules.txt

bin_SCRIPTS = sametlit.plx sl-cuneify.plx sl-db.plx sl-install.sh sl-pdf.sh \
	sl-se.plx sl-xml.plx slmanager.plx slsig.plx sl-sortcodes.plx \
	rep-filter.plx rep-sort.plx rep-xml.plx sl-BaseC.plx \
	sl-values-index.sh sl-values-index-tex.sh slse-slave.plx \
	sl-group.plx sl-signlist.sh

cgibin_SCRIPTS=slse
texdata_DATA = slmac.tex repmac.tex

scriptdata_DATA = sl-TeX.xsl rep-sl.xsl sl-HTML.xsl sl-sort.xsl \
	sl-number.xsl sl-missing-sort.xsl rep-TeX.xsl \
	sl-ESP-structure.xsl sl-ESP-letters.xsl sl-ESP-signs.xsl \
	sl-letters-file.xsl sl-tsv.xsl sl-compounds.xsl

slse: slse.in
	$(ORACCIFY) slse.in

slse-slave.plx: slse-slave.plx.in
	$(ORACCIFY) slse-slave.plx.in

slsig.plx: slsig.plx.in
	$(ORACCIFY) slsig.plx.in

sametlit.plx: sametlit.plx.in
	$(ORACCIFY) sametlit.plx.in

EXTRA_DIST = \
	Base.pm \
	gsl-TeX.xsl \
	gslmac.tex \
	nsl-db.plx \
	sametlit.plx.in \
	sl-HTML.xsl \
	sl-cuneify.plx \
	sl-db.plx \
	sl-install.sh \
	sl-pdf.sh \
	sl-se.plx \
	sl-xml.plx \
	slmanager.plx
