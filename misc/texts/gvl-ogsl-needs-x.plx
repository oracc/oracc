#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my %x = ();

while (<>) {
    if (/\s(\S+?ₓ) unknown/) {
	my $v = $1;
	/Known for (\S+?):\s/ || /known values for (\S+)\s/;
	my $q = $1;
	++${$x{$q}}{$v};
    }
}

foreach my $q (keys %x) {
    my @v = keys %{$x{$q}};
    print "$q\t@v\n";
}

1;
