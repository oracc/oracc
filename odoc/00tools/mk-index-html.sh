#!/bin/sh
WWW=$2
top=$3
mkdir -p $2
for a in index*.xml ; do
    xsltproc --stringparam doco-type $1 --stringparam prefix $top \
	--xinclude ../00tools/mk-index.xsl $a \
        >$2/`basename $a xml`html ; \
done
`dirname $0`/docperms.sh ${WWW}
