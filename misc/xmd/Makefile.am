include ../../oracc-am-rules.txt

bin_SCRIPTS = catvec.plx packsortinfo.plx Qtab.plx xmd-ids.plx xmdfields.plx xmdmanager2.plx \
	install.sh royal-sort.plx xcat.plx xmd-proxy.plx xmd-check.plx P-lookup.plx \
	merge-sorts.plx tsvfix.plx tsvclean.plx tsvcheck.plx xmd-sessions.plx
scriptdata_DATA = g2-xmd-HTML.xsl qcat-HTML.xsl royal-sort.xsl ruler-field.xsl \
	vals2labels.xsl xmd-format.xsl designation-sort.xsl xmd-HTML.xsl \
	ruler-records.xsl xmd-summaries.xsl xmd-HTML-summaries.xsl
data_DATA = keydata.xml
schema_DATA = fmpwww2.rnc imgdb.rnc

xmd-ids.plx: xmd-ids.plx.in
	$(ORACCIFY) xmd-ids.plx.in

xmdfields.plx: xmdfields.plx.in
	$(ORACCIFY) xmdfields.plx.in

catvec.plx: catvec.plx.in
	$(ORACCIFY) catvec.plx.in

xmdmanager2.plx: xmdmanager2.plx.in
	$(ORACCIFY) xmdmanager2.plx.in

packsortinfo.plx: packsortinfo.plx.in
	$(ORACCIFY) packsortinfo.plx.in

Qtab.plx: Qtab.plx.in
	$(ORACCIFY) Qtab.plx.in

EXTRA_DIST = \
	CSV.pm \
	Fields.pm \
	Q.pm \
	Qtab.plx.in \
	SortCodes.pm \
	SortLabels.pm \
	Util.pm \
	catvec.plx.in \
	fmpwww2.rnc \
	g2-xmd-HTML.xsl \
	imgdb.rnc \
	install.sh \
	keydata.xml \
	packsortinfo.plx.in \
	qcat-HTML.xsl \
	royal-sort.plx \
	royal-sort.xsl \
	ruler-field.xsl \
	vals2labels.xsl \
	xcat.plx \
	xmd-HTML.xsl \
	xmd-format.xsl \
	xmd-ids.plx.in \
	xmd-proxy.plx \
	xmdfields.plx.in \
	xmdmanager2.plx.in
