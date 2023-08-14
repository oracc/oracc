#!/bin/sh
tr -s '\t' ' ' | tr -s '\n' | grep -v ^# | grep -v '@end form' | grep -v '^[ 	]*$' | sed 's/form ~[a-z]\+/form/' | sort -u
