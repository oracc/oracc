include ../../oracc-am-rules.txt

bin_SCRIPTS = oid.plx oid-index-cbd.sh oid-resolver.plx xis-resolver.plx oiduniq.plx \
	gloid.plx edit2oid.plx oidedit2xml.plx xis-xml-page.sh xis-xml-html.sh

html_DATA = xis-api.html

scriptdata_DATA = oid-history.xsl

xis-resolver.plx: xis-resolver.plx.in
	$(ORACCIFY) xis-resolver.plx.in
