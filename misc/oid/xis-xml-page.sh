#!/bin/sh
proj=$1
lang=$2
inst=$3
tisfile=$ORACC_BUILDS/pub/$proj/cbd/$lang/$lang.tis
xisdb $tisfile $inst | wm -p proj -l | ce_xtf -3 -p proj -l 
