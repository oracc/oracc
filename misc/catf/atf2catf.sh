#!/bin/sh
if [ "$1" == "" ];
then 
    echo atf2catf.sh: must give name of ATF file
    exit 
fi
atf=`basename $1 .atf`
ox $atf.atf -l= | catf-manager.plx -s >$atf.catf
ox -cv $atf.catf 2>$atf-catf.log
wc -l $atf-catf.log

