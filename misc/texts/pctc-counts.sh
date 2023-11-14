#!/bin/sh
#sloid.sh pctc | pctc-count-data.sh \
#    | tee 02pub/sl-corpus-counts.tab \
#    | rocox -x counts -f \
#	    >$ORACC_BUILDS/pctc/02xml/sl-corpus-counts.xml
(cd 01tmp/xis ;
 c=counts.xml
 echo '<counts>' >$c
 for a in o* ; do
     if [ -r $a/data ]; then
	 echo '<o xml:id="'$a'">'>>$c
	 grep -v '^oid' $a/data | rocox -x c >>$c
	 echo '</o>' >>$c
     fi
 done
 echo '</counts>' >>$c
)
mv  01tmp/xis/counts.xml 02xml/counts.xml
xsltproc $ORACC_BUILDS/lib/scripts/sx-add-sort.xsl 02xml/counts.xml \
	 >02xml/sl-corpus-counts.xml
chmod o+r 02xml/sl-corpus-counts.xml
rm -f 02xml/counts.xml
