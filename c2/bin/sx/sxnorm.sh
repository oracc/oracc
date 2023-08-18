#!/bin/sh
tr -s '\t' ' ' | tr -s '\n' | grep -a -v ^@utf8 | grep -a -v '^@@$' | grep -a -v '@end form' \
    | grep -a -v '^[ 	]*$' | sed 's/form ~[a-z]\+/form/' | sort -u
