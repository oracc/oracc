#!/bin/sh
/bin/echo '\beginvalindex' >valindex.tex
xsltproc ~/oracc/misc/sl/sl-values.xsl sorted.xml | psdsort | perl -C -n -e 'chomp;my($k,$v)=split(/\t/,$_); print "\\ix{$k}{$v}\n"' >>valindex.tex
/bin/echo '\endvalindex' >>valindex.tex
