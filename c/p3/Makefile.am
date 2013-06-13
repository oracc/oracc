include ../../oracc-am-rules.txt

subdirs=ccgi-1.1

bin_SCRIPTS=p3-project-data.plx p3-project.sh p3-pager.plx
cgibin_PROGRAMS=p3
css_DATA=p3.css
data_DATA=p3-template.xml
js_DATA=p3.js
scriptdata_DATA=p3-project.xsl p3-ce-HTML.xsl p3-htmlview.xsl p3-xmd-div.xsl

p3-pager.plx: p3-pager.plx.in
	$(ORACCIFY) p3-pager.plx.in

p3-xmd-div.xsl: p3-xmd-div.xsl.in
	$(ORACCIFY) p3-xmd-div.xsl.in

p3_SOURCES=p3.c \
	ccgi-1.1/libccgi.a

p3_LDADD=ccgi-1.1/libccgi.a

ccgi-1.1/libccgi.a:
	(cd ccgi-1.1 ; make)

extra_DIST=\
	Slicer.pm
