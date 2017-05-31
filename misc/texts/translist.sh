#!/bin/sh
if [ -d '00atf' ]; then
   find 00atf -type f -print0 | xargs -0 grep -h '^\(&\|@translation\)' \
       | perl -pe \
       'chomp;s/^\&(.......).*$/\n$1/||s/^\S+\s+\S+\s+(\S+)\s+\S+\s*$/ $1/' \
       | sort | grep -v ^$
fi
