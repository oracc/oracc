include ../../oracc-am-rules.txt

bin_SCRIPTS = unfxr.plx
schema_DATA = fxr.rnc

unfxr.plx: unfxr.plx.in
	$(ORACCIFY) unfxr.plx.in

EXTRA_DIST = \
	Util.pm \
	fxr.rnc \
	unfxr.plx.in
