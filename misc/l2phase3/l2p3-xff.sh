#!/bin/sh
webdir=$1
l=$2
echo xff=yes
xffdir=$webdir/cbd/$l/xff
rm -fr $xffdir
art2xff.plx -d $xffdir -l sux
chmod +rx $webdir/cbd/$l/xff
chmod -R o+r $webdir/cbd/$l/xff
echo after art2xff
