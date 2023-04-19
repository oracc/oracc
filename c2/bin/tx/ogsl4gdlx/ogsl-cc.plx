#!/usr/bin/perl
use warnings; use strict; use open ':utf8'; use utf8;
binmode STDIN, 'utf8'; binmode STDOUT, 'utf8'; binmode STDERR, 'utf8';

my %seen = ();
while (<>) {
    s/^(.*?)\s//;
    print unless $seen{$1}++;
}

1;
