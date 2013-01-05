include ../../oracc-am-rules.txt

scriptdata_DATA = log-HTML.xsl

log-HTML.xsl: log-HTML.xsl.in
	$(ORACCIFY) log-HTML.xsl.in

EXTRA_DIST = \
	Util.pm \
	log-HTML.xsl.in
