#!/bin/sh
$ORACC/bin/agg-list-public-projects.sh | perl -e \
    '@p=(<>);chomp @p; map {printf("$_ %s\n", (stat("$ENV{qw/ORACC_BUILDS/}/$_/00lib/config.xml"))[9])} @p'
