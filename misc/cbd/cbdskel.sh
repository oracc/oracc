#!/bin/sh
grep '^[+-]\?@\(proj\|lang\|name\|entry\|sense\)' $* | grep -v '^+' | perl -p -e 's/^-?(\@[a-z]+)\S*\s+/$1\t/'
