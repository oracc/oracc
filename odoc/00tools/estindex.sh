#!/bin/sh
d=$1
x=$1/estindex
rm -fr $x
${ORACC}/bin/estcmd create -tr -xs $x
find ${ORACC}/www/doc ${ORACC}/www/ns \
    | grep -v '\.[a-z0-9]\+\.' | grep -v cuneitab | \
    ${ORACC}/bin/estcmd gather $x -
chmod -R o+r $x
