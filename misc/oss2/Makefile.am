include ../../oracc-am-rules.txt

bin_SCRIPTS = oss.plx

oss.plx: oss.plx.in
	$(ORACCIFY) oss.plx.in

EXTRA_DIST = \
	Log.pm \
	Manager.pm \
	Node.pm \
	XCL.pm \
	oss.plx.in
