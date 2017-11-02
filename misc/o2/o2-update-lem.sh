#!/bin/sh
o2-cnf.sh
l2p1-from-glos.sh
l2p1-lemm-data.plx -update
(cd 02pub ; 
 for a in lemm-*.new ; do
     sig=`basename $a .new`.sig
     mv $a $sig
 done
 )
