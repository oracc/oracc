#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use ORACC::L2GLO:Util;

GetOptions(
    );

my %o = ();

while (<>) {
    chomp;
    s/\t.*$//;
    my %s = parse_sig($_);
    my $l = $s{'lang'}; $l =~ s/-.*$//;
    my $w = "$s{'cf'}\[$s{'gw'}\]$s{'pos'}\tword";
    ++${$o{$l}{$w}};
    my $s = "$s{'cf'}\[$s{'gw'}//$s{'sense'}\]$s{'pos'}'$s{'epos'}\tsense\t$w";
    ++${$o{$l}{$s}};
}

foreach my $l (keys %o) {
    foreach my $o (keys %{$o{$l}}) {
	print "$l\t$o\n";
    }
}

1;
