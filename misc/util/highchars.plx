#!/usr/bin/perl
use warnings; use strict; use utf8;
binmode STDIN, ':utf8';
binmode STDOUT, ':utf8';

while (<>) {
    s/[\200-\377][\200-\377]+//g;
    tr/\000-\177//d;
    if (length) {
	warn("$ARGV:$.: non-Unicode high-bit character\n");
    }
}
