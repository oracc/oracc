#/bin/sh
lang=$1
webdir=$2

if [ "$lang" == "" ]; then
    echo g2plus-make-all-indexes.sh: must give language as first argument
    exit 1
fi

if [ "$webdir" == "" ]; then
    echo g2plus-make-all-indexes.sh: must give webdir as second argument
    exit 1
fi
mkdir -p $webdir/cbd/$lang
for a in akkadian cf english signnames translit VS VC VCV ; do \
    g2plus-make-index.sh $a $lang $webdir ; \
done
xsltproc --stringparam lang $lang --stringparam project `oraccopt` \
    $ORACC/lib/scripts/g2plus-toc-matrix.xsl \
    $ORACC/lib/data/dummy.xtl >$webdir/cbd/$lang/toc-matrix.html
#$webdir/cbd/$lang/toc.html
chmod -R o+r $webdir/cbd/$lang
