#!/bin/sh
ORACC=$1 ; export ORACC
project=$2 ; export project
PATH=$ORACC/bin:$PATH ; export PATH
cd $ORACC/$project ; p2-project-data.plx
