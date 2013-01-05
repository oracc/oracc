#!/bin/sh
cd
onebig.plx
xsltproc --xinclude tools/cbd-selector.xsl project.xml \
    | xsltproc tools/cbd-strip-cpd-refs.xsl - >tmp/index.cbd
xsltproc tools/cbd-sortkeys.xsl tmp/index.cbd >tmp/indexkeys.txt
gdlsort.plx -x tmp/indexkeys.txt >tmp/indexcodes.xml
xsltproc --stringparam codesfile indexcodes.xml \
    tools/cbd-sortcodes.xsl tmp/index.cbd >tmp/indexcoded.cbd
xsltproc tools/cbd-sort.xsl tmp/indexcoded.cbd >tmp/indexsorted.cbd
xsltproc tools/index-HTML.xsl tmp/indexsorted.cbd >web/gloss.html
cp tools/gloss.css web/
chmod o+r web/gloss.css web/gloss.html
