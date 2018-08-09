include ../../oracc-am-rules.txt

bin_SCRIPTS = harvest-links.plx proj-linkbase.sh tlit-sort.sh cleanlinks.plx \
	ls1R2lst.plx atflinks.plx linknorm.plx \
	translist.sh proxtrans.sh fixtilde.plx \
	lex-sign-data.sh lex-sign-xtf.sh \
	lex-word-data.sh lex-word-get-data.sh lex-word-xtf.sh \
	lex-word-base.sh lex-word-qakk.sh lex-word-phra.sh \
	lex-master.sh

scriptdata_DATA = \
	lex-sign-data.xsl lex-sign-group.xsl lex-sign-lookup.xsl lex-sign-HTML.xsl \
	lex-word-data.xsl \
	lex-word-base-group.xsl lex-word-base-HTML.xsl \
	lex-word-qakk-group.xsl lex-word-qakk-HTML.xsl \
	lex-word-phra-pull.xsl lex-word-phra-group.xsl lex-word-phra-sort.xsl lex-word-phra-HTML.xsl \
	lex-refs-group.xsl

lex-sign-lookup.xsl: lex-sign-lookup.xsl.in
	$(ORACCIFY) lex-sign-lookup.xsl.in

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
