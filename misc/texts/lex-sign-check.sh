#!/bin/sh
lex-oid-sign.plx 2>los.log
grep 'no OID for sign' los.log | msort -r'\n' -d: -n1 -cN -n2 >bad-cpd.log
echo '&P121212 = X' >cpd.atf
echo '#project: epsd2' >>cpd.atf
echo '#atf: use unicode' >>cpd.atf
echo '#atf: use math' >>cpd.atf
sed 's/^.* for sign //' <bad-cpd.log | sort -u | perl -n -e '++$x; print "$x. $_"' >>cpd.atf
grep -c '^[0-9]' cpd.atf
ox -f cpd.atf | xsltproc $ORACC_BUILDS/lib/scripts/sl-compounds.xsl - >sl-compounds.log
