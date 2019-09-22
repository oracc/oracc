#!/bin/sh
if [ "$1" == "" OR "$2" == "" OR "$3" ]; then
    echo usage:
    echo "	s2 LEMMA BASE FORM"
sma2.plx -no-ext -lemma $1 -base $2 -form $3

