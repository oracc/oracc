include ../../oracc-am-rules.txt

bin_SCRIPTS = xcl2xst.plx xst2pcs.plx
scriptdata_DATA = xst-HTML.xsl xst2pcs.xsl xst2ppcs.xsl

xcl2xst.plx: xcl2xst.plx.in
	$(ORACCIFY) xcl2xst.plx.in

xst2pcs.plx: xst2pcs.plx.in
	$(ORACCIFY) xst2pcs.plx.in

EXTRA_DIST = \
	Manager.pm \
	xcl2xst.plx.in \
	xst-HTML.xsl \
	xst2pcs.plx.in \
	xst2pcs.xsl \
	xst2ppcs.xsl
