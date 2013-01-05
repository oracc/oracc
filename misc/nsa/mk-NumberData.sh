#!/bin/sh
rm -f NumberData.pm
xsltproc --xinclude nm-sys-seq.xsl nm/nm.xml | sed 's/disz/asz, disz/' \
    > nm.asc
cut -d: -f1 nm.asc >nm.asc.1
cut -d: -f2-10000 nm.asc | atf2uni.plx >nm.asc.2
paste -d: nm.asc.1 nm.asc.2 >nm.data
rm -f nm.asc*
cat NumberData.src nm.data >NumberData.pm
chmod -w NumberData.pm
