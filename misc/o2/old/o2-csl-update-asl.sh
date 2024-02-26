#!/bin/sh
csld=01bld/csl.d
mkdir -p $csld
if [ -r 00lib/corpus.asl ]; then
    cp 00lib/corpus.asl $csld
else
    (cd $csld
     cat ../lists/proxy-atf.lst | oxg >G.tab
     cut -f7 G.tab | tr ' ' '\n' | sort -u >rep.lst
     tlitsig.plx <rep.lst >rep.sig
     csl-rep-invert.plx -n rep.sig
     grep -F -f not-in-ogsl.lst G.tab >not-in-ogsl-inst.tab
     csl-non-ogsl-atf.plx
     ox -f -x= -l= not-in-ogsl.atf
     xsltproc -o not-in-ogsl.tab ~/orc/lib/scripts/sl-compounds.xsl not-in-ogsl.xml
     # use corrected compounds derived from not-in-ogsl.lst
     # generate rep.q
     csl-rep-invert.plx rep.sig
     csl-merge-forms.plx rep.q >rep.tab 
     csl-tab2asl.plx rep.tab | sl-list-from-ogsl.plx -s >corpus.asl )
fi
