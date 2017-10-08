#!/bin/sh
# Invoke with project as arg 1, PQX as arg 2
${ORACC}/bin/g2-xtf2txh.plx -htm -proj $1 $2
${ORACC}/bin/web-PQX-html.plx -htm -proj $1 $2
