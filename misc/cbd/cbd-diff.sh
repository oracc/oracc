#!/bin/sh
d1=$1
d2=$2
if [ "$d2" = "" ]; then
    echo "$0: must give two glossaries to diff. Stop."
    exit 1
else
    echo "$0: diffing '$d1' and '$d2'"
    cbdentries.plx -nomi -nono -nofo -noma $d1 >d1.entries
    cbdentries.plx -nomi -nono -nofo -noma $d2 >d2.entries
    diff d1.entries d2.entries >cbd.diff
    less cbd.diff
fi
