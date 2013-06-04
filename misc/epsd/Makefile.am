include ../../oracc-am-rules.txt

db-unxml.plx: db-unxml.plx.in
	$(ORACCIFY) db-unxml.plx.in

epsd: epsd.in
	$(ORACCIFY) epsd.in

epsd-se-indexer.plx: epsd-se-indexer.plx.in
	$(ORACCIFY) epsd-se-indexer.plx.in

epsdsl: epsdsl.in
	$(ORACCIFY) epsdsl.in

xff: xff.in
	$(ORACCIFY) xff.in

bin_SCRIPTS=epsd-se-indexer.plx db-unxml.plx
cgibin_SCRIPTS=epsd epsdsl xff
scriptdata_DATA=xff-HTML.xsl

EXTRA_DIST=\
	Compiler.pm
	DBM.pm
	Display.pm
	Evaluator.pm
