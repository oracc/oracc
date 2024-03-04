#!/bin/sh
/bin/echo '<?xml version="1.0" encoding="utf-8"?>' >valindex.xml
/bin/echo -n '<vindex>' >>valindex.xml
xsltproc ~/oracc/misc/sl/sl-values.xsl x.xml | psdsort | perl -C -n -e 'chomp;my($k,$v)=split(/\t/,$_); print "<ix><k>$k</k><v>$v</v></ix>"' >>valindex.xml
/bin/echo -n '</vindex>' >>valindex.xml
