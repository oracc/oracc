#!/bin/sh
grep -vF '(lem)         ' atf.log | grep -vF '#' | perl -n -e 's/^.*?=(.*?\]\S+).*$/$1/ && print' | sort
