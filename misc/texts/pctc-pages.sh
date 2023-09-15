#!/bin/sh
#
# Make portal pages relating to PCSL/PCTC/APUP
#
lib=$ORACC_BUILDS/lib/data
pub=$ORACC_BUILDS/pctc/02pub

grep '	0	' $pub/sl-corpus-counts.tab | cut -f1 >02pub/sl-corpus-none.lst
grep -v '	0	' $pub/sl-corpus-counts.tab | cut -f1 | grep -v '^o$' >02pub/sl-corpus-some.lst

xsltproc /home/pctc/00bin/ap-yes.xsl 02xml/sl.xml >02pub/ap-yes.lst
xsltproc /home/pctc/00bin/ap-not.xsl 02xml/sl.xml >02pub/ap-not.lst

# PCSL items not in AP but occcuring in corpus
grep -f 02pub/ap-not.lst 02pub/sl-corpus-some.lst >02pub/pcsl-not-ap-yes-corpus.lst

## PCSL items not in AP and not occurring in corpus
#grep -f 02pub/ap-not.lst 02pub/sl-corpus-none.lst >02pub/pcsl-not-ap-not-corpus.lst

# AP items in PCSL but not in corpus
grep -f 02pub/ap-yes.lst 02pub/sl-corpus-none.lst >02pub/pcsl-yes-ap-not-corpus.lst

for a in 02pub/pcsl-*.lst ; do
    t=`basename $a lst`tab
    slx -p pctc <$a | paste $a - >02pub/$t
done

h=02www/not-ap-yes-corpus.html
sed 's/@@TITLE@@/PCSL NOT IN AP/' <$lib/pctc-xis-h.html >$h
echo '<table>' >>$h
rocox -t <02pub/pcsl-not-ap-yes-corpus.tab >>$h
echo '</table>' >>$h
cat $lib/pctc-xis-t.html >>$h

h=02www/yes-ap-not-corpus.html
sed 's/@@TITLE@@/AP NOT CORPUS/' <$lib/pctc-xis-h.html >$h
echo '<table>' >>$h
rocox -t <02pub/pcsl-yes-ap-not-corpus.tab >>$h
echo '</table>' >>$h
cat $lib/pctc-xis-t.html >>$h

chmod o+r 02www/*.html
