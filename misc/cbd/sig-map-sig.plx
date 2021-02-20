#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use ORACC::L2GLO:Util;

# Read a list of signatures from the map file; must match CF[GW//SENSE]POS'EPOS
# Now read a list of signatures to map; replace matching subsigs from the map.

use Getopt::Long;

my $m = '';
my %m = ();

GetOptions(
    'map:s'=>\$m,
    );

open(M,$m) || die "$0: can't open map file $m\n";
while (<M>) {
    chomp;
    my($from,$to) = split(/\t/,$_);
    ++m{$from} = $to;
}
close(M);

while (<>) {
    chomp;
    my($pre,$core,$post) = (m,^(.*?=)(.*?)(/.*)$,);
    if ($m{$core}) {
	print "$pre$m{$core}$post\n";
    }
}

1;
