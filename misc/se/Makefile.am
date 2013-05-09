include ../../oracc-am-rules.txt

bin_SCRIPTS = s2-ce_trim.plx unit-context.plx s2-debug.sh s2-pg2wrapper.plx s2-pipeline.sh

s2-ce_trim.plx: s2-ce_trim.plx.in
	$(ORACCIFY) s2-ce_trim.plx.in

s2-debug.sh: s2-debug.sh.in
	$(ORACCIFY) s2-debug.sh.in

s2-pg2wrapper.sh: s2-pg2wrapper.sh.in
	$(ORACCIFY) s2-pg2wrapper.sh.in

s2-pipeline.sh: s2-pipeline.sh.in
	$(ORACCIFY) s2-pipeline.sh.in

unit-context.plx: unit-context.plx.in
	$(ORACCIFY) unit-context.plx.in

EXTRA_DIST = \
	s2-ce_trim.plx.in \
	s2-debug.sh,in \
	s2-pg2wrapper.plx.in \
	s2-pipeline.sh.in \
	unit-context.plx.in
