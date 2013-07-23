include ../../oracc-am-rules.txt

bin_SCRIPTS = web-PQX-html.plx web-default-index.plx web-p2colours.plx clean-web-PQX-log.sh
scriptdata_DATA = web-default-index.xsl

web-PQX-html.plx: web-PQX-html.plx.in
	$(ORACCIFY) web-PQX-html.plx.in

EXTRA_DIST = \
	web-PQX-html.plx.in \
	web-default-index.plx \
	web-default-index.xsl \
	web-p2colours.plx
