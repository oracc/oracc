#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use Data::Dumper;

my %args = @ARGV;

print Dumper \%args;

1;
