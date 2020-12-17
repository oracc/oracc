#!/bin/sh
rm -f elog-homog.lst
grep -F '[3]' entries.log | cut -d: -f4 | perl -p -e 's/^\s*(.*?)\s+unknown.*$/$1/' | tr -s '\n' >elog-homog.lst
grep -F '[4]' entries.log | cut -d: -f5 | perl -p -e 's/^\s*//' >>elog-homog.lst
cat elog-homog.lst | entry-homographs.plx >homographs.log
