#!/bin/sh
grep -h '^&' 00atf/*.atf | perl -p -e 's/^.\s*(\S+)\s*=\s*/$1\t/' >00lib/names.tab
