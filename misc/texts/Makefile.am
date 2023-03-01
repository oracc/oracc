include ../../oracc-am-rules.txt

bin_SCRIPTS = harvest-links.plx proj-linkbase.sh tlit-sort.sh cleanlinks.plx \
	ls1R2lst.plx atflinks.plx linknorm.plx \
	translist.sh proxtrans.sh fixtilde.plx \
	lex-sign-data.sh lex-sign-data-2.sh lex-sign-xtf.sh \
	lex-word-data.sh lex-word-get-data.sh lex-word-xtf.sh \
	lex-word-base.sh lex-word-qakk.sh lex-word-phra.sh lex-word-equi.sh \
	lex-master.sh lex-data-tis.sh lex-phra-base.sh lex-tis.sh \
	tis-sort.plx lex-word-tail.sh lex-oid-sign.plx lex-oid-word.plx \
	lex-sign-check.sh lex-wid2err.sh err-sort.plx wid2lem_sigs.plx \
	inst-fix-tab.plx makemake.plx form-inst.sh cbd-lem-fix.plx \
	cbd-lem-fix-apply.plx base-inst.sh locdata.sh \
	ngram-matches.plx ngram-html-report.plx ngram-log-err-format.plx \
	wid2err.plx err-split.plx lex-provides-tab.plx lex-xincludes.plx \
	gvl-ogsl-needs-uncomp.plx gvl-ogsl-needs-x.plx gvl-fix-should-be.plx

scriptdata_DATA = \
	lex-sign-data.xsl lex-sign-group.xsl lex-sign-lookup.xsl lex-sign-HTML.xsl \
	lex-word-data.xsl lex-data-line.xsl \
	lex-word-base-group.xsl lex-word-base-HTML.xsl \
	lex-word-phra-pull.xsl lex-word-phra-group.xsl lex-word-phra-sort.xsl lex-word-phra-HTML.xsl \
	lex-refs-group.xsl lex-sort.xsl \
	lex-data-xis-ids.xsl lex-data-xis-remap.xsl lex-data-xis.xsl \
	lex-pull-base.xsl lex-phra-base-group.xsl lex-phra-base-HTML.xsl \
	lex-provides.xsl lex-split-HTML.xsl \
	lex-pull-tail.xsl lex-phra-tail-group.xsl lex-merge-phra.xsl \
	lex-word-equi-HTML.xsl

lex-sign-lookup.xsl: lex-sign-lookup.xsl.in
	$(ORACCIFY) lex-sign-lookup.xsl.in

lex-word-data.xsl: lex-word-data.xsl.in
	$(ORACCIFY) lex-word-data.xsl.in

proj-linkbase.sh: proj-linkbase.sh.in
	$(ORACCIFY) proj-linkbase.sh.in

tlit-sort.sh: tlit-sort.sh.in
	$(ORACCIFY) tlit-sort.sh.in

harvest-links.plx: harvest-links.plx.in
	$(ORACCIFY) harvest-links.plx.in

EXTRA_DIST = \
	cleanlinks.plx \
	harvest-links.plx.in \
	ls1R2lst.plx \
	proj-linkbase.sh.in \
	tlit-sort.sh.in
