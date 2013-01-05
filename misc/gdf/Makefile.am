include ../../oracc-am-rules.txt

bin_SCRIPTS = gdf-entry.sh gdf-sort.sh gdf-update.sh
css_DATA = gdf.css
scriptdata_DATA = gdf-entry-html.xsl gdf-est.xsl gdf-ids.xsl gdf-index-html.xsl gdf-sort.xsl
data_DATA = gdf-template.xml

gdf-entry.sh: gdf-entry.sh.in
	$(ORACCIFY) gdf-entry.sh.in

EXTRA_DIST = \
	gdf-entry-html.xsl \
	gdf-entry.sh.in \
	gdf-est.xsl \
	gdf-ids.xsl \
	gdf-index-html.xsl \
	gdf-sort.sh \
	gdf-sort.xsl \
	gdf-template.xml \
	gdf-update.sh \
	gdf.css
