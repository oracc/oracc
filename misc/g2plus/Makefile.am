include ../../oracc-am-rules.txt

bin_SCRIPTS = epsd-se-indexer.plx g2plus-index-db.plx g2plus-make-all-indexes.sh g2plus-make-index.sh g2plus-sort-index.plx lexindex.plx mk-all-print-indexes.sh mk-print-index.sh usgindex.plx
scriptdata_DATA = g2plus-html-toc.xsl g2plus-index-akkadian.xsl g2plus-index-bases.xsl g2plus-index-cf.xsl g2plus-index-driver.xsl g2plus-index-english.xsl g2plus-index-es.xsl g2plus-index-html-print.xsl g2plus-index-signnames.xsl g2plus-index-toc.xsl g2plus-index-translit.xsl g2plus-merge-index-summaries.xsl g2plus-merge-index-wheres.xsl g2plus-toc-matrix.xsl g2plus-index-VS.xsl g2plus-index-VC.xsl g2plus-index-VCV.xsl

g2plus-index-toc.xsl: g2plus-index-toc.xsl.in
	$(ORACCIFY) g2plus-index-toc.xsl.in

EXTRA_DIST = \
	epsd-se-indexer.plx \
	g2plus-html-toc.xsl \
	g2plus-index-akkadian.xsl \
	g2plus-index-bases.xsl \
	g2plus-index-cf.xsl \
	g2plus-index-VS.xsl \
	g2plus-index-VC.xsl \
	g2plus-index-VCV.xsl \
	g2plus-index-db.plx \
	g2plus-index-driver.xsl \
	g2plus-index-english.xsl \
	g2plus-index-es.xsl \
	g2plus-index-html-print.xsl \
	g2plus-index-signnames.xsl \
	g2plus-index-toc.xsl.in \
	g2plus-index-translit.xsl \
	g2plus-make-all-indexes.sh \
	g2plus-make-index.sh \
	g2plus-merge-index-summaries.xsl \
	g2plus-merge-index-wheres.xsl \
	g2plus-sort-index.plx \
	g2plus-toc-matrix.xsl \
	lexindex.plx \
	mk-all-print-indexes.sh \
	mk-print-index.sh \
	usgindex.plx
