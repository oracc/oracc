include ../../oracc-am-rules.txt

scriptdata_DATA = oss-l-ids.xsl
bin_SCRIPTS = oss.plx oss-check-l.sh

oss.plx: oss.plx.in
	$(ORACCIFY) oss.plx.in

EXTRA_DIST = \
	Log.pm \
	Manager.pm \
	Node.pm \
	XCL.pm \
	oss.plx.in
