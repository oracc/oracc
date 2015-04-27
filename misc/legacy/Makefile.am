include ../../oracc-am-rules.txt

bin_SCRIPTS = add-PQX.plx akklang.sh change-project.sh default-link-defs.plx \
	fix-saa-num.plx fix-semi.plx \
	fix-sense-chars.plx fix-strip-sux-norm.plx fix-sux-cf-aleph.plx \
	fix-sux-cont.plx fix-sux-glo.plx killbom.plx mapchars.plx mapstyle.plx \
	sexify.plx style-info.plx unodf.sh unspace-filenames.plx unspace-filenames.sh \
	odt2xml.sh text-extractor.plx odt-simplifier.plx \
	no-simplify.sh no-fixchars.sh

scriptdata_DATA = StyleInfo.xsl xml2txt.xsl odt2xml.xsl

odt-simplifier.plx: odt-simplifier.plx.in
	$(ORACCIFY) odt-simplifier.plx.in

text-extractor.plx: text-extractor.plx.in
	$(ORACCIFY) text-extractor.plx.in

EXTRA_DIST = \
	Charmap.pm \
	ODF.pm \
	Sexify.pm \
	StyleFonts.pm \
	StyleInfo.pm \
	StyleInfo.xsl \
	add-PQX.plx \
	akklang.sh \
	change-project.sh \
	fix-saa-num.plx \
	fix-semi.plx \
	fix-sense-chars.plx \
	fix-strip-sux-norm.plx \
	fix-sux-cf-aleph.plx \
	fix-sux-cont.plx \
	fix-sux-glo.plx \
	killbom.plx \
	mapchars.plx \
	mapstyle.plx \
	sexify.plx \
	style-info.plx \
	unodf.sh \
	unspace-filenames.plx \
	unspace-filenames.sh
