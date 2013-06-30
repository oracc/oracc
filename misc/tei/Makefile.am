include ../../oracc-am-rules.txt

bin_SCRIPTS = tei-project.sh teitexts.sh tei-bigfile.sh tei-dist.sh tei-merge.sh \
	tei-no-trim.sh tei-trim.sh tei-xmd-headers.sh teimanager.plx \
	tei-translations.sh
css_DATA = xtf2tei.css
scriptdata_DATA = xtf2tei.xsl cbd2listNym.xsl cbd2listPerson.xsl prev-cbd2listNym.xsl \
	tei-xmd-headers.xsl teicorpus-header.xsl teicorpus.xsl teirefs.xsl \
	trim-listNym.xsl trim-tei-project.xsl xmd2tei.xsl r2tei.xsl

html_DATA = teicolours.html

teitexts.sh: teitexts.sh.in
	$(ORACCIFY) teitexts.sh.in

tei-project.sh: tei-project.sh.in
	$(ORACCIFY) tei-project.sh.in

xtf2tei.xsl: xtf2tei.xsl.in
	$(ORACCIFY) xtf2tei.xsl.in

EXTRA_DIST = \
	cbd2listNym.xsl \
	cbd2listPerson.xsl \
	prev-cbd2listNym.xsl \
	tei-bigfile.sh \
	tei-dist.sh \
	tei-merge.sh \
	tei-no-trim.sh \
	tei-project.sh.in \
	tei-trim.sh \
	tei-xmd-headers.sh \
	tei-xmd-headers.xsl \
	teicolours.html \
	teicorpus-header.xsl \
	teicorpus.xsl \
	teimanager.plx \
	teirefs.xsl \
	teitexts.sh.in \
	trim-listNym.xsl \
	trim-tei-project.xsl \
	xmd2tei.xsl \
	xtf2tei.css \
	xtf2tei.xsl.in
