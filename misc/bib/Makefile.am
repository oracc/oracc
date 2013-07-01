include ../../oracc-am-rules.txt

bin_SCRIPTS = bibdbdump.plx biblcgi.plx biblcites.plx biblindex.plx \
	biblquery.plx biblsort.sh fixnamemap.plx ref2tei.sh tei-merge.sh \
	bibltext.plx bibltoxml.plx fixnames.plx ref2xml.plx unfxr-bib.sh
css_DATA = bib-reflist.css
scriptdata_DATA = addsigs.xsl biblist.xsl cite-note-TEXT.xsl ckey-abbrevs.xsl \
	ckey-namedate.xsl ckey-numbers.xsl CMS_A-HTML-driver.xsl CMS_A-HTML-each.xsl \
	CMS_A-HTML-embedded.xsl CMS_A-HTML.xsl harvest-names.xsl map-jrnser.xsl \
	map-name-fuzzy.xsl map-name.xsl merge-names.xsl name-key.xsl ref2tei.xsl \
	sort-bib.xsl sort-list.xsl sort-names.xsl sort-refs.xsl tei-merge.xsl tei-xmlid.xsl

data_DATA = common.xml jrnser.xml names-pre-fixmap.xml names.xml onames.xml

biblsort.sh: biblsort.sh.in
	$(ORACCIFY) biblsort.sh.in

biblindex.plx: biblindex.plx.in
	$(ORACCIFY) biblindex.plx.in

tei-merge.sh: tei-merge.sh.in
	$(ORACCIFY) tei-merge.sh.in

biblquery.plx: biblquery.plx.in
	$(ORACCIFY) biblquery.plx.in

biblcites.plx: biblcites.plx.in
	$(ORACCIFY) biblcites.plx.in

bibdbdump.plx: bibdbdump.plx.in
	$(ORACCIFY) bibdbdump.plx.in

fixnamemap.plx: fixnamemap.plx.in
	$(ORACCIFY) fixnamemap.plx.in

ref2tei.sh: ref2tei.sh.in
	$(ORACCIFY) ref2tei.sh.in

biblcgi.plx: biblcgi.plx.in
	$(ORACCIFY) biblcgi.plx.in

EXTRA_DIST = \
	CMS_A-HTML-driver.xsl \
	CMS_A-HTML-each.xsl \
	CMS_A-HTML-embedded.xsl \
	CMS_A-HTML.xsl \
	CiteResolver.pm \
	Config.pm \
	Index.pm \
	Indexer.pm \
	KeyMangler.pm \
	QueryContext.pm \
	addsigs.xsl \
	bib-reflist.css \
	bibdbdump.plx.in \
	biblcgi.plx.in \
	biblcites.plx.in \
	biblindex.plx.in \
	biblist.xsl \
	biblquery.plx.in \
	biblsort.sh.in \
	bibltext.plx \
	bibltoxml.plx \
	cite-note-TEXT.xsl \
	ckey-abbrevs.xsl \
	ckey-namedate.xsl \
	ckey-numbers.xsl \
	common.xml \
	fixnamemap.plx.in \
	fixnames.plx \
	harvest-names.xsl \
	jrnser.xml \
	map-jrnser.xsl \
	map-name-fuzzy.xsl \
	map-name.xsl \
	merge-names.xsl \
	name-key.xsl \
	names-pre-fixmap.xml \
	names.xml \
	onames.xml \
	ref2tei.sh.in \
	ref2tei.xsl \
	ref2xml.plx \
	sort-bib.xsl \
	sort-list.xsl \
	sort-names.xsl \
	sort-refs.xsl \
	tei-merge.sh.in \
	tei-merge.xsl \
	tei-xmlid.xsl
