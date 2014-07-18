#!/bin/sh
for a in `find . -name '*.zip'` ; do
    cp -p $a ${ORACC}/www/downloads ; 
done
chmod o+r ${ORACC}/www/downloads/*
