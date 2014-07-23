include ../../oracc-doc-rules.txt
SUBDIRS = basics neworacc ports prereqs
EXTRA_DIST = index.html

install-data-local:
	rm -fr ${htmldir}/install
	mkdir -p ${htmldir}/install
	cp index.html ${htmldir}/install
	chmod o+r ${htmldir}/install/index.html
