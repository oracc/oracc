#!/bin/sh
o2-unbuild.sh
touch 01tmp/building
oracc rebuild
o2-logfiles.sh
rm -f 01tmp/building
