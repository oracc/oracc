include ../../oracc-am-rules.txt

bin_SCRIPTS = esp2-image-info.sh esp2-init.sh esp2-live.sh esp2-lmt.plx \
	esp2-no-nmtok-err.plx esp2-remove-webfiles.sh esp2.sh esp2-uninstall.sh \
	esp-to-esp2.sh esp2-unoracc.sh esp2-p3colours.plx esp2-create.sh \
	esp2-stylesheets.plx

scriptdata_DATA = esp2-alphabet.xslt esp2-chars-iso-8859-1.xslt esp2-chars-markup-internationalization.xslt \
	esp2-chars-symbols-maths-greek.xslt esp2-dumb-quotes.xslt esp2-functions.xslt esp2-menu.xslt \
	esp2-phase-01.xslt esp2-phase-02.xslt esp2-phase-03.xslt esp2-phase-04.xslt esp2-phase-05.xslt \
	esp2-references.xslt esp2-saxon-collate.xslt esp2-site-map.xslt \
	esp2-fix-sort.xsl esp2-params.xsl esp2-home.xsl

data_DATA = esp2-structure.xml

java_DATA = ImageInfo.class

libesp2=$(DESTDIR)$(prefix)/lib/esp2

ImageInfo.class: ImageInfo.java
	javac ImageInfo.java

install-data-local:
	rm -fr $(libesp2)
	mkdir -p $(libesp2)
	scp -r css $(libesp2)
	scp -r images $(libesp2)
	scp -r js $(libesp2)

EXTRA_DIST = \
	ImageInfo.class \
	blank.gif \
	esp2-alphabet.xslt \
	esp2-chars-iso-8859-1.xslt \
	esp2-chars-markup-internationalization.xslt \
	esp2-chars-symbols-maths-greek.xslt \
	esp2-dumb-quotes.xslt \
	esp2-functions.xslt \
	esp2-init.sh \
	esp2-live.sh \
	esp2-lmt.plx \
	esp2-menu.xslt \
	esp2-no-nmtok-err.plx \
	esp2-phase-01.xslt \
	esp2-phase-02.xslt \
	esp2-phase-03.xslt \
	esp2-phase-04.xslt \
	esp2-phase-05.xslt \
	esp2-references.xslt \
	esp2-remove-webfiles.sh \
	esp2-saxon-collate.xslt \
	esp2-site-map.xslt \
	esp2-stylesheets.xslt \
	esp2.sh \
	library.js \
	textsizeminus1.css \
	textsizeminus2.css \
	textsizeminus3.css \
	textsizeplus1.css \
	textsizeplus2.css \
	textsizeplus3.css \
	textsizeplus4.css \
	textsizeplus5.css
