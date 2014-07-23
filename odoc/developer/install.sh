#!/bin/sh
WWW=${ORACC}/www/doc/developer
for  a in `find . -name '*.xdf'`; do \
    d=`dirname $a` ; echo $d ; \
    mkdir -p ${WWW}/$d ; \
    (cd $d ; xdfmanager.plx ; \
        cp -a html/* ${WWW}/$d ; \
    rm -fr html) \
done
../00tools/mk-index.plx
../00tools/mk-index-html.sh developer ${WWW}
