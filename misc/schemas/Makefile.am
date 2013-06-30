include ../../oracc-am-rules.txt

bin_SCRIPTS = install.sh mk-dist-schemas.sh
java_DATA = trang.jar
data_DATA = schemas.xml
schema_DATA =  	od-manifest-schema-v1.2-cs01.rnc od-schema-v1.2-cs01.rnc xslt20.rnc

install-data-local:
	cp -pR xhtml $(DESTDIR)$(prefix)/lib/schemas

EXTRA_DIST = \
	install.sh \
	od-manifest-schema-v1.2-cs01.rnc \
	od-schema-v1.2-cs01.rnc \
	mk-dist-schemas.sh \
	schemas.xml \
	trang.jar \
	xslt20.rnc \
	xhtml/*
