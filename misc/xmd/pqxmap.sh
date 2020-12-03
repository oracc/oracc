#!/bin/sh
project=$1
pqx=$2
grep $2 $ORACC_BUILDS/pub/$project/cat/pqids.lst
