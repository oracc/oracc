#!/bin/sh
wid2lem <01bld/lists/have-xtf.lst | gzip -9 >loc-data-`date +%Y%m%d`.gz
wid2lem <01bld/lists/have-xtf.lst | xz >loc-data-`date +%Y%m%d`.xz
