#!/usr/bin/perl
use warnings; use strict;

my %l = ();

while (<>) {
    /6\]: .*?=(.*?):\s/ && ++$l{$1};
}

print map { "$_\t$_\n" } sort keys %l;

1;
