include ../../oracc-am-rules.txt

bin_SCRIPTS = sma.plx morph.plx

sma.plx: sma.plx.in
	$(ORACCIFY) sma.plx.in

EXTRA_DIST = \
	Base.pm \
	Display.pm \
	Graphinfo.pm \
	ISF.pm \
	NSF.pm \
	Number.pm \
	ParseManual.pm \
	Rootless.pm \
	StripNSF.pm \
	StripVPR.pm \
	VPR.pm \
	VSF.pm \
	sma.plx.in
