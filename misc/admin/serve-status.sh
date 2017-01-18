#!/bin/sh
$ORACC/bin/agg-list-public-projects.sh | perl -e \
    '@p=(<>);chomp @p; map {$f="$ENV{qw/ORACC_BUILDS/}/$_/00lib/config.xml";-f $f && printf("$_ %s\n", (stat($f))[9])} @p'
