#!/bin/sh
wid2lem <01bld/lists/have-xtf.lst | gzip -9 >02pub/loc-data-`date +%Y%m%d`.gz
