#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my %seen = ();
my @x = ();
my %x = ();

while (<>) {
    push @x, $_;
    ++$x{$_};
}

foreach my $x (@x) {
    print "$x{$x}\t$x" unless $seen{$x}++;
}

1;
