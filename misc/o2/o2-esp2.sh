#!/bin/sh
o2-cnf.sh || exit 1
live=$1
if [ "$live" == "" ]; then
    ${ORACC}/bin/esp2.sh `oraccopt`
elif [ "$live" == "live" ]; then
    shift
    ${ORACC}/bin/esp2-live.sh `oraccopt` force
    o2-weblive.sh
else
    echo oracc esp takes one optional argument which must be "'live'"
    exit 1
fi
exit 0
