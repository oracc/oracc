#!/bin/sh
OPROJECT=`oraccopt` ; export OPROJECT
rm -f 01bld/cancel
o2-super-prep.sh
o2-glossary.sh
