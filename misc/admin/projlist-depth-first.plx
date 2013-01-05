#!/usr/bin/perl
use warnings; use strict;

my @configs = map { s,^$ENV{'ORACC'}/xml/(.*?)/config.xml$,$1,; $_ } `find $ENV{'ORACC'}/xml -follow -name config.xml`;

print sort { $b =~ tr,/,/, <=> $a =~ tr,/,/, } @configs;

1;
