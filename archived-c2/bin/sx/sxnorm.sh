#!/bin/sh
tr -s '\t' ' ' | tr -s '\n' | grep -a -v ^@utf8 | grep -a -v '^@@$' |  grep -a -v '^[ 	]*$' | sort -u
