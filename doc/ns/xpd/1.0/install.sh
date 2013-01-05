#!/bin/sh
cdoc=~/www/configdoc
java -jar ${ORACC}/lib/java/trang.jar xpd-source.rnc xpd-source.rng
xsltproc xpd-INSTANCE.xsl xpd-source.rng >default.xpd
xmllint --xinclude xpd-XFORM.xsl | xsltproc - xpd-source.rng >~/www/test-config.xml
rm -fr configdoc ; mkdir -p configdoc
xsltproc xpd-HTML.xsl xpd-source.rng >/dev/null #./configdoc/index.html
rm -fr $cdoc
mv configdoc $cdoc
cp -f config.css ~/www
chmod o+r ~/www/test-config.xml ~/www/config.css
chmod -R o+r $cdoc
