#!/bin/sh
if [ -d '00atf' ]; then
    find 00atf -type f -print0 | xargs -0 grep -h '\@translation\|^\#tr\.' | perl -p -e 's/^#tr\.([a-z]+):.*$/. . $1/' | cut -d' ' -f3 | sort -u | tr '\n' ' '
fi
