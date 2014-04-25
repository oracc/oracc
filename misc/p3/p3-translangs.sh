#!/bin/sh
if [ -d '00atf' ]; then
   find 00atf -type f -print0 | xargs -0 grep -h '\@translation' | cut -d' ' -f3 | sort -u | tr '\n' ' '
fi
