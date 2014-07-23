#!/bin/sh
./metrol-tables.plx >metrol-tables.html
WWW=${ORACC}/www/doc/builder
d=${WWW}/metrology
mkdir -p $d
xdfmanager.plx metrology.xdf ; cp -a html/* $d ; rm -fr html
