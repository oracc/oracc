#!/bin/sh
tr -s '\t' ' ' | tr -s '\n' | grep -v ^# | grep -v '@end form' | grep -v '^[ 	]*$' | sort -u
