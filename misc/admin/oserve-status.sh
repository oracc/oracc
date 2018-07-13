#!/bin/sh
$ORACC/bin/agg-list-public-projects.sh | perl -e \
    '@p=(<>);chomp @p; map {$f="$ENV{qw/ORACC_BUILDS/}/$_/servestamp";-f $f && printf("$_ %s\n", (stat($f))[9])} @p'
