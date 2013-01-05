include ../../oracc-am-rules.txt

bin_SCRIPTS = test-xcf.plx

test-xcf.plx: test-xcf.plx.in
	$(ORACCIFY) test-xcf.plx.in

EXTRA_DIST = \
	Util.pm \
	test-xcf.plx.in
