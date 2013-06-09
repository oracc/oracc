include ../../oracc-am-rules.txt

subdirs=ccgi-1.1

bin_SCRIPTS=p3-project-data.plx p3-project.sh p3-pager.plx
cgibin_PROGRAMS=oracc
css_DATA=p3.css
data_DATA=p3-template.xml
js_DATA=p3.js
scriptdata_DATA=p3-project.xsl p3-ce-HTML.xsl

p3-pager.plx: p3-pager.plx.in
	$(ORACCIFY) p3-pager.plx.in

p3-slicer.plx: p3-slicer.plx.in
	$(ORACCIFY) p3-slicer.plx.in

oracc_SOURCES=oracc.c \
	ccgi-1.1/libccgi.a

oracc_LDADD=ccgi-1.1/libccgi.a

ccgi-1.1/libccgi.a:
	(cd ccgi-1.1 ; make)

extra_DIST=\
	Slicer.pm
