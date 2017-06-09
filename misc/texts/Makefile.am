include ../../oracc-am-rules.txt

bin_SCRIPTS = harvest-links.plx proj-linkbase.sh tlit-sort.sh cleanlinks.plx \
	ls1R2lst.plx atflinks.plx linknorm.plx \
	translist.sh proxtrans.sh fixtilde.plx

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
