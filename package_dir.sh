#!/bin/sh
grep ^package $1 | perl -pe 's/^.*?(ORACC::(?:[a-zA-Z0-9]+::)*).*$/$1/' | sed 's/::/\//g' | sed 's/\/$//'
