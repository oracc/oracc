include ../../oracc-am-rules.txt

bin_SCRIPTS = harvest-links.plx proj-linkbase.sh tlit-sort.sh cleanlinks.plx \
	ls1R2lst.plx atflinks.plx linknorm.plx \
	translist.sh proxtrans.sh fixtilde.plx \
	err-sort.plx wid2lem_sigs.plx \
	inst-fix-tab.plx makemake.plx form-inst.sh cbd-lem-fix.plx \
	cbd-lem-fix-apply.plx base-inst.sh locdata.sh \
	ngram-matches.plx ngram-html-report.plx ngram-log-err-format.plx \
	wid2err.plx err-split.plx \
	gvl-ogsl-needs-uncomp.plx gvl-ogsl-needs-x.plx gvl-fix-should-be.plx \
	pctc-signs.sh pctc-xis.plx pctc-divs.sh pctc-xis-data.sh tmp-xis-web.sh \
	pctc-alldata.sh pctc-count-data.sh pctc-counts.sh pctc-pages.sh

data_DATA=pctc-xis-*.html

scriptdata_DATA = \
	pctc-add-oids.xsl word-children.xsl

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
