include ../../oracc-am-rules.txt

bin_SCRIPTS = l2p1-simple.plx l2p1-bigrams.sh l2p1-cofs.plx l2p1-from-glos.sh \
	l2p1-from-xtfs.plx l2p1-lemm-data.plx l2p1-needs-update.plx \
	l2p1-pre-atf.sh l2p1-project-sig.plx l2p1-proxy-xtf-data.plx l2p1-psus.plx \
	l2p1-sig-merger.plx l2p1-sux-norm.plx l2p1.sh \
	l2p1-sig-invert.plx l2p1-project-from-union.sh l2p1-prune-sigs.plx \
	l2p1-remap-sigs.plx l2p1-simple-from-project.sh l2p1-union-ranks.plx

l2p1-simple.plx: l2p1-simple.plx.in
	$(ORACCIFY) l2p1-simple.plx.in

EXTRA_DIST = \
	l2p1-bigrams.sh \
	l2p1-cofs.plx \
	l2p1-from-glos.sh \
	l2p1-from-xtfs.plx \
	l2p1-lemm-quick-freqs.plx \
	l2p1-needs-update.plx \
	l2p1-pre-atf.sh \
	l2p1-project-sig.plx \
	l2p1-proxy-xtf-data.plx \
	l2p1-psus.plx \
	l2p1-sig-merger.plx \
	l2p1-simple.plx.in \
	l2p1-sux-norm.plx \
	l2p1-union.plx \
	l2p1.sh
