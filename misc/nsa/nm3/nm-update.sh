#!/bin/sh
./nm2xml.plx NM2.txt >nm.xml
cp -f nm.xml ${ORACC}/lib/data
chmod o+r ${ORACC}/lib/data/nm.xml
