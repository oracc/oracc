#!/bin/sh
cd 00atf ; mkdir -p new
rm -f new/*
for a in *.atf ; do  sed "s/lang akk$/lang akk-x-$1/" <$a >new/$a ; done
