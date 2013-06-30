include ../../oracc-am-rules.txt

bin_SCRIPTS = linkmap.plx non-dynamic-cat-ids.plx center-image.sh clean_nl.plx \
	config-inherit.sh config-uninherit.sh config.plx cph.plx createproj.plx \
	glo-lang.sh lst2xml.plx ncr ncr-sudo.sh nor oracc project-data-sudo.sh \
	project-stats.plx xpq.sh oxc \

css_DATA = stats.css
scriptdata_DATA = subconfig.xsl config-inherit.xsl copy-options.xsl identity.xsl stats-HTML.xsl xpd.xsl
data_DATA = linkids.xml sll_link_ids.xml

oxc: oxc.in
	$(ORACCIFY) oxc.in

subconfig.xsl: subconfig.xsl.in
	$(ORACCIFY) subconfig.xsl.in

non-dynamic-cat-ids.plx: non-dynamic-cat-ids.plx.in
	$(ORACCIFY) non-dynamic-cat-ids.plx.in

linkmap.plx: linkmap.plx.in
	$(ORACCIFY) linkmap.plx.in

EXTRA_DIST = \
	Util.pm \
	center-image.sh \
	clean_nl.plx \
	config-inherit.sh \
	config-inherit.xsl \
	config-uninherit.sh \
	config.plx \
	copy-options.xsl \
	cph.plx \
	createproj.plx \
	glo-lang.sh \
	identity.xsl \
	linkids.xml \
	linkmap.plx.in \
	lst2xml.plx \
	ncr \
	ncr-sudo.sh \
	non-dynamic-cat-ids.plx.in \
	nor \
	oracc \
	oracc-install.sh \
	project-data-sudo.sh \
	project-stats.plx \
	sll_link_ids.xml \
	stats-HTML.xsl \
	stats.css \
	subconfig.xsl.in \
	xpd.xsl \
	xpq.sh
