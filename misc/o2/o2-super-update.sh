#!/bin/sh
superglo=`o2-cbdpp.sh`
cp $superglo 01tmp/superglo-for-lemmdata.glo
l2p1-lemm-data.plx -g 01tmp/superglo-for-lemmdata.glo
