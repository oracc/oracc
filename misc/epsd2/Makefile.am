include ../../oracc-am-rules.txt

bin_SCRIPTS = epsd2-merge-forms.plx epsd2-init.sh epsd2-epadify.plx \
	art2xff.plx epsd2-pull-forms.plx epsd2-rebuild.sh \
	epsd2-u3adm-sigs.plx epsd2-update.sh epsd2-to-l2p2.sh \
	super-check-glo.sh super-build-subs.sh \
	emesalify.plx xff.plx

xff.plx: xff.plx.in
	$(ORACCIFY) xff.plx.in

scriptdata_DATA = signdata.xsl xff-HTML.xsl
EXTRA_DIST = 
