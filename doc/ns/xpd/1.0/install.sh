#!/bin/sh
cdoc=${ORACC}/www/configdoc
java -jar ${ORACC}/lib/java/trang.jar xpd-source.rnc xpd-source.rng
xsltproc xpd-INSTANCE.xsl xpd-source.rng >default.xpd
xmllint --xinclude xpd-XFORM.xsl | xsltproc - xpd-source.rng >${ORACC}/www/test-config.xml
rm -fr configdoc ; mkdir -p configdoc
xsltproc xpd-HTML.xsl xpd-source.rng >/dev/null #./configdoc/index.html
rm -fr $cdoc
mv configdoc $cdoc
cp -f config.css ${ORACC}/www
chmod o+r ${ORACC}/www/test-config.xml ${ORACC}/www/config.css
chmod -R o+r $cdoc
