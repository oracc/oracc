#!/bin/sh
if [ -r 00lib/derived.lst ]; then
    atflists.plx -U 01bld/lists/lemindex.lst - 00lib/derived.lst | \
	bigrams -p `oraccopt` >02pub/lemm-bigrams.ngm
else
    bigrams -p `oraccopt` >02pub/lemm-bigrams.ngm <01bld/lists/have-lem.lst
fi
