#!/usr/bin/perl
use warnings; use strict;

my %l = ();

while (<>) {
    /lemma (.*?)\s*not found/ && ++$l{$1};
}

print map { "$_\n" } sort keys %l;

1;
