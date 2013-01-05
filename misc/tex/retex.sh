#!/bin/sh
# Create the TeX formats and ensure the TeX loadables are installed
cp tex/*.tex ${ORACC}/lib/tex ; chmod -R o+r ${ORACC}/lib/tex
mk-oracctex.sh
