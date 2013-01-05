#!/bin/sh
WWW=${ORACC}/www/ns
rm -f {esp,xdf}/1.0/xhtml
ln -sf ${ORACC}/lib/schemas/xhtml esp/1.0/xhtml
ln -sf ${ORACC}/lib/schemas/xhtml xdf/1.0/xhtml
# Note that xtr gets built twice, but this is benign--we
# just want to make sure that xtr is ready when xtf needs
# it.
for a in xtr/1.0 */* ; do \
    (cd $a ; echo $a ; xdfmanager.plx ; rm -fr ${WWW}/$a ; mkdir -p ${WWW}/$a ; mv html/* ${WWW}/$a) \
done

../00tools/mk-index.plx */*/*.xdf \
    | xsltproc --stringparam doco-type Schema --param ns 'true()' -xinclude ../00tools/mk-index.xsl \
    index.xml \
    >${WWW}/index.html

# Install all of the .rnc schemas in oracc/lib/schemas.
echo installing schemas ...
find . -name '*.rnc' -type f -exec cp -pR '{}' ${ORACC}/lib/schemas ';'
chmod -R o+r ${ORACC}/lib/schemas
chmod -R o+r ${WWW}/*/*
