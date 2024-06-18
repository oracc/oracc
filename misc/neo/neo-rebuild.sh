#!/bin/sh
echo Building NEO ...
neo-proxies.plx
cut -d' ' -f1 <00lib/neo.lst >00lib/proxy.lst
o2-lst.sh
mkdir -p 01bld/cat
xmd-proxy.plx
xmdmanager2.plx
xsltproc ${ORACC}/lib/scripts/xmd-summaries.xsl 01bld/cdlicat.xmd >01bld/cat-sum.xml
xsltproc ${ORACC}/lib/scripts/xmd-HTML-summaries.xsl 01bld/cat-sum.xml >01bld/cat-sum.html
neo-sigs.plx
neo-oids.sh
l2p1-simple-from-project.sh
l2p2.sh
o2-web.sh
l2p3.sh 01bld/www

#run all the indexing for neo
qindex.sh

for a in 02pub/cbd/* ; do
    lang=`basename $a`
    if [ -r 01bld/$lang/letter_ids.tab ]; then
	cp 01bld/$lang/letter_ids.tab $a
	cp 01bld/$lang/L*.lst $a
    fi
done
o2-weblive.sh
o2-prm.sh
neo-cbd-json.sh

mkdir -p 02www/lists
grep -v ^neo: 01bld/lists/outlined.lst >02www/lists/outlined.lst
chmod -R o+r 02www/lists

pxp3p4.sh `oraccopt`
pxreset.sh `oraccopt`

#docker restart oracc-ingest

#o2-json.sh
#neo-portals.sh
