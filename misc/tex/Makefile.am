include ../../oracc-am-rules.txt

bin_SCRIPTS = mk-oracctex.sh cdftexmanager.plx oracctex retex.sh texmanager.plx
texdata_DATA = cdlrandom.tex chartest.tex cuneify.tex facinglines.tex gdl.tex \
	newmcol.tex onewmcol.tex orinap.tex pagecont.tex rinap.tex tet-mac.tex xtf.tex \
	cdf/*.tex cdf/*.ini cdf/graphics/*.tex
scriptdata_DATA = csl-TeX.xsl cuneify-TeX.xsl glo-TeX.xsl TeX-util.xsl
data_DATA = chartest.xml

mk-oracctex.sh: mk-oracctex.sh.in
	$(ORACCIFY) mk-oracctex.sh.in

EXTRA_DIST = \
	TeX-util.xsl \
	cdftexmanager.plx \
	cdlrandom.tex \
	chartest.tex \
	chartest.xml \
	csl-TeX.xsl \
	cuneify-TeX.xsl \
	cuneify.tex \
	facinglines.tex \
	gdl.tex \
	glo-TeX.xsl \
	mk-oracctex.sh.in \
	newmcol.tex \
	onewmcol.tex \
	oracctex \
	orinap.tex \
	pagecont.tex \
	retex.sh \
	rinap.tex \
	tet-mac.tex \
	texmanager.plx \
	xtf.tex
