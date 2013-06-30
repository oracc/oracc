include ../../oracc-am-rules.txt

bin_SCRIPTS = agg-list-public-projects.sh agg-project-list.sh agg-thumbs.sh \
	agg-test.sh agg-editions.sh agg-linkbase.sh \
	agg-master-xtl.plx agg-proxify.plx
scriptdata_DATA = agg-html-projects.xsl agg-index.xsl agg-list-public-projects.xsl \
	agg-public-image.xsl agg-public-projects.xsl url.xsl

agg-master-xtl.plx: agg-master-xtl.plx.in
	$(ORACCIFY) agg-master-xtl.plx.in

agg-proxify.plx: agg-proxify.plx.in
	$(ORACCIFY) agg-proxify.plx.in

EXTRA_DIST = \
	agg-html-projects.xsl \
	agg-index.xsl \
	agg-list-public-projects.sh \
	agg-list-public-projects.xsl \
	agg-project-list.sh \
	agg-proxify.plx.in \
	agg-public-image.xsl \
	agg-public-projects.xsl \
	agg-thumbs.sh \
	url.xsl
