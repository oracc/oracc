include ../../oracc-am-rules.txt

bin_SCRIPTS = odtpictures.plx otf2odt.sh otf2pdf.sh otf2tex.sh odfmanager.plx \
	otf2odt-serial.sh otf2odt-plain.sh otf2odt-tab.sh otf2div.sh \
	atf2pdf.sh atf-driver.sh pdf-driver.sh webpdf.sh clipdf.sh

config_DATA = default.sty office-automatic-styles.xml office-master-styles.xml \
	office-styles.xml
scriptdata_DATA = bib-ODT.xsl cat-ODT.xsl doc-split.xsl gdl-ODT.xsl \
	odt-table-width.xsl odt-TeX.xsl odt-xpd.xsl oracc-odtTeX.xsl otf-ODT.xsl xtf-ODT.xsl \
	otf-ODT-parallel.xsl \
	otf-ODT-parallel-lib.xsl \
	otf-ODT-serial.xsl \
	otf-ODT-serial-lib.xsl \
	cat-ODT-serial.xsl \
	xtf-ODT-serial.xsl \
	otf-ODT-plain.xsl \
	otf-ODT-plain-lib.xsl \
	otf-HTML-div.xsl \
	cat-ODT-plain.xsl \
	xtf-ODT-plain.xsl \
	xtr-ODT-plain.xsl \
	xtr-ODT.xsl \
	web-pdf-xmd.xsl

atf2pdf.sh: atf2pdf.sh.in
	$(ORACCIFY) atf2pdf.sh.in

atf-driver.sh: atf-driver.sh.in
	$(ORACCIFY) atf-driver.sh.in

webpdf.sh: webpdf.sh.in
	$(ORACCIFY) webpdf.sh.in

clipdf.sh: clipdf.sh.in
	$(ORACCIFY) clipdf.sh.in

odfmanager.plx: odfmanager.plx.in
	$(ORACCIFY) odfmanager.plx.in

oracc-odtTeX.xsl: oracc-odtTeX.xsl.in
	$(ORACCIFY) oracc-odtTeX.xsl.in

pdf.plx: pdf.plx.in
	$(ORACCIFY) pdf.plx.in

EXTRA_DIST = \
	bib-ODT.xsl \
	cat-ODT.xsl \
	default.sty \
	doc-split.xsl \
	gdl-ODT.xsl \
	odt-TeX.xsl \
	odt-table-width.xsl \
	odt-xpd.xsl \
	odtpictures.plx \
	office-automatic-styles.xml \
	office-master-styles.xml \
	office-styles.xml \
	oracc-odtTeX.xsl.in \
	otf-ODT.xsl \
	otf2odt.sh \
	otf2pdf.sh \
	otf2tex.sh \
	odfmanager.plx.in \
	xtf-ODT.xsl \
	xtr-ODT.xsl \
	otf-ODT-parallel.xsl \
	otf-ODT-parallel-lib.xsl \
	otf-ODT-serial.xsl \
	otf-ODT-serial-lib.xsl \
	xtf-ODT-serial.xsl \
	cat-ODT-serial.xsl
