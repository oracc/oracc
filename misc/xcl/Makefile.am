include ../../oracc-am-rules.txt

bin_SCRIPTS = xcl2ptb.plx xcl2uxh.plx xcl2xtr.plx xclmanager.plx
scriptdata_DATA = merge-XLF.xsl test.xsl xcl-HTML.xsl xcl-XTR.xsl xcl2ptb.xsl xcl-json.xsl

xcl2xtr.plx: xcl2xtr.plx.in
	$(ORACCIFY) xcl2xtr.plx.in

xcl2uxh.plx: xcl2uxh.plx.in
	$(ORACCIFY) xcl2uxh.plx.in

xclmanager.plx: xclmanager.plx.in
	$(ORACCIFY) xclmanager.plx.in

xcl2ptb.plx: xcl2ptb.plx.in
	$(ORACCIFY) xcl2ptb.plx.in

EXTRA_DIST = \
	merge-XLF.xsl \
	test.xsl \
	xcl-HTML.xsl \
	xcl-XTR.xsl \
	xcl2ptb.plx.in \
	xcl2ptb.xsl \
	xcl2uxh.plx.in \
	xcl2xtr.plx.in \
	xclmanager.plx.in
