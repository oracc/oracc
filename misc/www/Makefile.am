include ../../oracc-am-rules.txt

SUBDIRS=util

js_DATA = cbd.js notes.js oraccdoc.js outline.js winlib.js xdf.js
scriptdata_DATA = corpusview.xsl ce-xtr-HTML.xsl cunei-used.xsl		\
	cunei-used2.xsl cuneify-HTML.xsl cuneify-string.xsl cuneify-text.xsl	\
	html-standard.xsl html-template.xsl html-util.xsl textdisplay.xsl	\
	xmdoutline.xsl
bin_SCRIPTS = atf.plx cdlhtml.plx cfgwview.plx cuneify.plx		\
	htmlimage.plx htmlview.plx oase.plx oracchtml.plx rpc.plx atflog50.sh	\
	atfpaste.sh datestamp.plx htmlbase.plx index-downloads.plx \
	atftemplate.plx voltemplate.plx weblists.sh
css_DATA = anyproj.css atfedit.css cbd.css ctxt.css cuneiform.css	\
	cuneify-mixed.css cuneify-na.css cuneify-ob.css cuneify.css		\
	cuneitab.css doc.css epsd.css help.css image.css na.css nafilled.css	\
	ncdl.css ncdlprint.css oraccbase.css oraccpod.css oraccprint.css	\
	oraccscreen.css orig-SHU-white.css outline.css ppcs-print.css		\
	ppcs.css pqpager.css preformat.css psdview.css score-block.css		\
	SHU-white.css signlist.css stve.css undefined.css x.css xdf-print.css	\
	xmd.css xo.css xtl.css oracchome.css oraccscreencss.xml oraccbasecss.xml \
	cuneiformcss.xml oracc3home.css wwwhome.css oraccdoc.css
img_DATA = blogspot-icon.jpg blogspot-icon.png breaks.png		\
	cdli-icon.png cdli.png dbl-l-arrow.png dbl-r-arrow.png f_logo.png	\
	lar40x22.png oclose.png oopen.png Qcat-icon.png rar40x22.png		\
	strut.png twitter-bird-dark-bgs.png twitter-bird-light-bgs.png		\
	xnum-icon.png up.gif down.gif viewtext.png oracc.jpg cc88x31.png	\
	p3-reset.png p3-help.png p3-search.png p3-plus.png \
	p3-pageitem.png p3-forward.png p3-backward.png p3-full.png \
	oracc32x32b.png oracc32x32w.png
html_DATA = blogspot16x16.ico facebook16x16.ico twitter16x16.ico no_html.xml \
	doc-redirect.html index.html no_html.html blanks.png favicon.ico
noinst_DATA = httpd.conf robots.txt

doc-redirect.html: doc-redirect.html.in
	cp doc-redirect.html.in doc-redirect.html

index.html: index.html.in
	cp index.html.in index.html

no_html.html: no_html.html.in
	cp no_html.html.in no_html.html

httpd.conf: httpd.conf.in
	$(ORACCIFY) httpd.conf.in

htmlview.plx: htmlview.plx.in
	$(ORACCIFY) htmlview.plx.in

oase.plx: oase.plx.in
	$(ORACCIFY) oase.plx.in

atftemplate.plx: atftemplate.plx.in
	$(ORACCIFY) atftemplate.plx.in

cuneify.plx: cuneify.plx.in
	$(ORACCIFY) cuneify.plx.in

oracchtml.plx: oracchtml.plx.in
	$(ORACCIFY) oracchtml.plx.in

cdlhtml.plx: cdlhtml.plx.in
	$(ORACCIFY) cdlhtml.plx.in

corpusview.xsl: corpusview.xsl.in
	$(ORACCIFY) corpusview.xsl.in

xmdoutline.xsl: xmdoutline.xsl.in
	$(ORACCIFY) xmdoutline.xsl.in

atf.plx: atf.plx.in
	$(ORACCIFY) atf.plx.in

rpc.plx: rpc.plx.in
	$(ORACCIFY) rpc.plx.in

htmlimage.plx: htmlimage.plx.in
	$(ORACCIFY) htmlimage.plx.in

cfgwview.plx: cfgwview.plx.in
	$(ORACCIFY) cfgwview.plx.in

html-standard.xsl: html-standard.xsl.in
	$(ORACCIFY) html-standard.xsl.in

oraccscreencss.xml: oraccscreen.css
	echo '<css>' >oraccscreencss.xml
	sed 's/\.css/css.xml/g' <oraccscreen.css >>oraccscreencss.xml
	echo '</css>' >>oraccscreencss.xml

cuneiformcss.xml: cuneiform.css
	echo '<css>' >cuneiformcss.xml
	cat cuneiform.css >>cuneiformcss.xml
	echo '</css>' >>cuneiformcss.xml

oraccbasecss.xml: oraccbase.css
	echo '<css>' >oraccbasecss.xml
	cat oraccbase.css >>oraccbasecss.xml
	echo '</css>' >>oraccbasecss.xml

EXTRA_DIST = \
	Qcat-icon.png \
	SHU-white.css \
	anyproj.css \
	atf.plx.in \
	atfedit.css \
	atflog50.sh \
	atfpaste.sh \
	blogspot-icon.jpg \
	blogspot-icon.png \
	blogspot16x16.ico \
	breaks.png \
	cbd.css \
	cbd.js \
	cdlhtml.plx.in \
	cdli-icon.png \
	cdli.png \
	ce-xtr-HTML.xsl \
	cfgwview.plx.in \
	corpusview.xsl.in \
	ctxt.css \
	cunei-used.xsl \
	cunei-used2.xsl \
	cuneiform.css \
	cuneify-HTML.xsl \
	cuneify-mixed.css \
	cuneify-na.css \
	cuneify-ob.css \
	cuneify-string.xsl \
	cuneify-text.xsl \
	cuneify.css \
	cuneify.plx.in \
	cuneitab.css \
	datestamp.plx \
	dbl-l-arrow.png \
	dbl-r-arrow.png \
	doc.css \
	epsd.css \
	f_logo.png \
	facebook16x16.ico \
	help.css \
	html-standard.xsl.in \
	html-template.xsl \
	html-util.xsl \
	htmlbase.plx \
	htmlimage.plx.in \
	htmlview.plx.in \
	httpd.conf \
	image.css \
	lar40x22.png \
	na.css \
	nafilled.css \
	ncdl.css \
	ncdlprint.css \
	notes.js \
	oase.plx.in \
	oclose.png \
	oopen.png \
	oraccbase.css \
	oraccdoc.js \
	oracchtml.plx.in \
	oraccpod.css \
	oraccprint.css \
	oraccscreen.css \
	orig-SHU-white.css \
	outline.css \
	outline.js \
	ppcs-print.css \
	ppcs.css \
	pqpager.css \
	preformat.css \
	psdview.css \
	rar40x22.png \
	robots.txt \
	rpc.plx.in \
	score-block.css \
	signlist.css \
	strut.png \
	stve.css \
	textdisplay.xsl \
	twitter-bird-dark-bgs.png \
	twitter-bird-light-bgs.png \
	twitter16x16.ico \
	undefined.css \
	voltemplate.plx \
	winlib.js \
	x.css \
	xdf-print.css \
	xdf.js \
	xmd.css \
	xmdoutline.xsl \
	xnum-icon.png \
	xo.css \
	xtl.css
