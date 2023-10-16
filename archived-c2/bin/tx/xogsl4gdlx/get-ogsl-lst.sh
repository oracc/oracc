#!/bin/sh
grep '^@\(sign\|form\|v\)' $ORACC/ogsl/00lib/ogsl.asl \
    | sed 's/^.*[ \t]//' \
    | grep '[aeiuAEIU]' \
    | grep -F -v '[' | grep -F -v ']' | grep -v '[-_{.ₓ!?]' | grep -v "'" | grep -v ^/ \
    | sort -u >ogsl.lst
