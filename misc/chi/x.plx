#!/usr/bin/perl
use warnings; use strict;
use Getopt::Long;

my @r = qw/has-x/;

use Data::Dumper; print Dumper @r;

my $x = 0;
GetOptions(
    'x:s'=>\$x
    );

print "$x\n";

1;
