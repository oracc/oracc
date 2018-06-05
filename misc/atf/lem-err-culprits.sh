#!/bin/sh
grep -vF '(lem)         ' $* >notab.lst
perl -n -e 's/^.*?=(.*?\]\S+).*$/$1/ && print' | sort -u
grep partial $* | perl -p -e 's/^.*?for\s+(.*?)\ in .*$/$1/' | sort -u

