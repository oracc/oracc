#!/bin/sh
WWW=$2
for a in index*.xml ; do
    xsltproc --stringparam doco-type $1 --xinclude ../00tools/mk-index.xsl $a \
        >$2/`basename $a xml`html ; \
done
../00tools/docperms.sh ${WWW}
