include ../../oracc-am-rules.txt

bin_SCRIPTS = esp-init.sh esp-live.sh esp-lmt.plx esp-no-nmtok-err.plx esp-remove-webfiles.sh esp.sh
css_DATA = textsizeminus1.css textsizeminus2.css textsizeminus3.css textsizeplus1.css textsizeplus2.css \
	textsizeplus3.css textsizeplus4.css textsizeplus5.css
js_DATA = library.js
scriptdata_DATA = esp-alphabet.xslt esp-chars-iso-8859-1.xslt esp-chars-markup-internationalization.xslt \
	esp-chars-symbols-maths-greek.xslt esp-dumb-quotes.xslt esp-functions.xslt esp-images-info.xslt \
	esp-menu.xslt esp-phase-01.xslt esp-phase-02.xslt esp-phase-03.xslt esp-phase-04.xslt esp-phase-05.xslt \
	esp-references.xslt esp-saxon-collate.xslt esp-site-map.xslt esp-stylesheets.xslt
img_DATA = blank.gif
java_DATA = ImageInfo.class

esp-images-info.xslt: esp-images-info.xslt.in
	$(ORACCIFY) esp-images-info.xslt.in

esp-live.sh: esp-live.sh.in
	$(ORACCIFY) esp-live.sh.in

esp-phase-01.xslt: esp-phase-01.xslt.in
	$(ORACCIFY) esp-phase-01.xslt.in

esp-phase-03.xslt: esp-phase-03.xslt.in
	$(ORACCIFY) esp-phase-03.xslt.in

esp-phase-04.xslt: esp-phase-04.xslt.in
	$(ORACCIFY) esp-phase-04.xslt.in

esp-stylesheets.xslt: esp-stylesheets.xslt.in
	$(ORACCIFY) esp-stylesheets.xslt.in

install-data-local:
	mkdir -p $(DESTDIR)$(prefix)/lib/esp
	scp -r 00web $(DESTDIR)$(prefix)/lib/esp

EXTRA_DIST = \
	ImageInfo.class \
	blank.gif \
	esp-alphabet.xslt \
	esp-chars-iso-8859-1.xslt \
	esp-chars-markup-internationalization.xslt \
	esp-chars-symbols-maths-greek.xslt \
	esp-dumb-quotes.xslt \
	esp-functions.xslt \
	esp-images-info.xslt.in \
	esp-init.sh \
	esp-live.sh.in \
	esp-lmt.plx \
	esp-menu.xslt \
	esp-no-nmtok-err.plx \
	esp-phase-01.xslt.in \
	esp-phase-02.xslt \
	esp-phase-03.xslt.in \
	esp-phase-04.xslt.in \
	esp-phase-05.xslt \
	esp-references.xslt \
	esp-remove-webfiles.sh \
	esp-saxon-collate.xslt \
	esp-site-map.xslt \
	esp-stylesheets.xslt.in \
	esp.sh \
	library.js \
	textsizeminus1.css \
	textsizeminus2.css \
	textsizeminus3.css \
	textsizeplus1.css \
	textsizeplus2.css \
	textsizeplus3.css \
	textsizeplus4.css \
	textsizeplus5.css
