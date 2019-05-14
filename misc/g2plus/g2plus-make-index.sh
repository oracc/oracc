#/bin/sh
type=$1
lang=$2
webdir=$3
if [ "$type" == "" ]; then
    echo g2plus-make-index.sh: must give type as first argument
    exit 1
fi
if [ "$lang" == "" ]; then
    echo g2plus-make-index.sh: must give language as second argument
    exit 1
fi
if [ "$webdir" == "" ]; then
    echo g2plus-make-index.sh: must give webdir as third argument
    exit 1
fi
mkdir -p $webdir/cbd/$lang
base=01bld/$lang/$type
bin=$ORACC/bin
xsl=$ORACC/lib/scripts
xsltproc -stringparam project `oraccopt` $xsl/g2plus-index-$type.xsl 01bld/$lang/articles.xml >$base.srt
$bin/g2plus-sort-index.plx $base.srt
xsltproc $xsl/g2plus-merge-index-wheres.xsl $base.srt >$base.xix
xsltproc $xsl/g2plus-merge-index-summaries.xsl $base.xix >$base-summaries.xix
xsltproc --stringparam base 01bld/$lang $xsl/g2plus-index-toc.xsl $base-summaries.xix >$base.toc
if [[ "$base" = *"V"* ]]; then
    withcount="yes"
else
    withcount="no"
fi
xsltproc --stringparam project `oraccopt` --stringparam withcount $withcount \
    $xsl/xix-HTML.xsl $base-summaries.xix > $webdir/cbd/$lang/`basename $base-summaries.xix xix`html
for a in $base-toc-*.xix ; \
    do echo $a ; xsltproc --stringparam project `oraccopt` \
			  $xsl/xix-HTML.xsl $a > $webdir/cbd/$lang/`basename $a xix`html ; \
    done
if [ "$type" = "signnames" ]; then
    xsltprox g2plus-signnames-homographs.xsl $webdir/cbd/$lang/`basename $base-summaries.xix xix`html \
	     >$webdir/cbd/$lang/signname-homographs.html
fi

#ln -sf `pwd`/indexes/$type.xix print/full
#rm -f tmp/$type.srt
