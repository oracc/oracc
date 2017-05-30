include ../../oracc-am-rules.txt

bin_SCRIPTS = g2-xtf2txh.plx scoregen.plx xtf2rtf.plx xtf2tei.plx finishcnc.plx xtfmanager.plx g2-xsf2sxh.plx
css_DATA = score.css xtf-common.css xtf-line.css xtf-para.css
scriptdata_DATA = xtf-HTML-lib.xsl escape-quotes.xsl scoreblock.xsl scoreblock2.xsl xtf-XCL.xsl xtf1-HTML.xsl \
	xtf-unscore.xsl sxh-view.xsl has-score.xsl has-sources.xsl lst-sources.xsl
data_DATA = label-info.xml

xtf2tei.plx: xtf2tei.plx.in
	$(ORACCIFY) xtf2tei.plx.in

g2-xsf2sxh.plx: g2-xsf2sxh.plx.in
	$(ORACCIFY) g2-xsf2sxh.plx.in

g2-xtf2txh.plx: g2-xtf2txh.plx.in
	$(ORACCIFY) g2-xtf2txh.plx.in

scoregen.plx: scoregen.plx.in
	$(ORACCIFY) scoregen.plx.in

xtf2rtf.plx: xtf2rtf.plx.in
	$(ORACCIFY) xtf2rtf.plx.in

xtf-HTML-lib.xsl: xtf-HTML-lib.xsl.in
	$(ORACCIFY) xtf-HTML-lib.xsl.in

EXTRA_DIST = \
	RTF.pm \
	escape-quotes.xsl \
	finishcnc.plx \
	g2-xtf2txh.plx.in \
	label-info.xml \
	score.css \
	scoreblock.xsl \
	scoreblock2.xsl \
	scoregen.plx.in \
	xtf-HTML-lib.xsl.in \
	xtf-XCL.xsl \
	xtf-common.css \
	xtf-line.css \
	xtf-para.css \
	xtf1-HTML.xsl \
	xtf2rtf.plx.in \
	xtf2tei.plx.in \
	xtfmanager.plx
