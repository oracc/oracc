include ../../oracc-am-rules.txt

bin_SCRIPTS = chi-un-id.plx chimanager.plx chirehead.plx chiremap.plx chireverse.sh \
	chi-test-checker.plx chi-test-fixer.plx chi-test-indexer.plx chi-test-loader.plx chi-test-maker.plx \
	chi-test-merger.plx chi-test-padder.plx chi-test-printer.plx chi-test-render.plx chi-test-sorter.plx \
	chi-test-splitter.plx

chi-test-checker.plx: chi-test-checker.plx.in
	$(ORACCIFY) chi-test-checker.plx.in

chi-test-indexer.plx: chi-test-indexer.plx.in
	$(ORACCIFY) chi-test-indexer.plx.in

chi-test-loader.plx: chi-test-loader.plx.in
	$(ORACCIFY) chi-test-loader.plx.in

chi-un-id.plx: chi-un-id.plx.in
	$(ORACCIFY) chi-un-id.plx.in

chimanager.plx: chimanager.plx.in
	$(ORACCIFY) chimanager.plx.in

EXTRA_DIST = \
	Checker.pm \
	Compare.pm \
	Exporter.pm \
	Fixer.pm \
	Importer.pm \
	Indexer.pm \
	Merger.pm \
	Padder.pm \
	Printer.pm \
	Reader.pm \
	Remap.pm \
	Sorter.pm \
	Splitter.pm \
	Writer.pm \
	chi-un-id.plx \
	chimanager.plx \
	chirehead.plx \
	chiremap.plx \
	chireverse.sh \
	chi-test-checker.plx \
	chi-test-fixer.plx \
	chi-test-indexer.plx \
	chi-test-loader.plx \
	chi-test-maker.plx \
	chi-test-merger.plx \
	chi-test-padder.plx \
	chi-test-printer.plx \
	chi-test-render.plx \
	chi-test-sorter.plx \
	chi-test-splitter.plx
