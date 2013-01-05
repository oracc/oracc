include ../../oracc-am-rules.txt

bin_SCRIPTS = s2-ce_trim.plx unit-context.plx s2-debug.sh s2-pg2wrapper.plx s2-pipeline.sh

s2-ce_trim.plx: s2-ce_trim.plx.in
	$(ORACCIFY) s2-ce_trim.plx.in

unit-context.plx: unit-context.plx.in
	$(ORACCIFY) unit-context.plx.in

EXTRA_DIST = \
	s2-ce_trim.plx.in \
	s2-debug.sh \
	s2-pg2wrapper.plx \
	s2-pipeline.sh \
	unit-context.plx.in
