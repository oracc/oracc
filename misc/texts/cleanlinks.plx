#!/usr/bin/perl -p
use warnings; use strict;

if (/^(?:<<|>>|\|\|)/) {
    s/\s+0+/ /g;
}

1;
