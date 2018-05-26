#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Hash;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

$ORACC::CBD::nonormify = 1;
$ORACC::CBD::noletters = 1;


my %args = pp_args();
my $h = undef;

my $inverted = $args{'invert'};

if (($h = pp_hash(\%args))) {
    my %acd = pp_hash_acd($h);
    my @sub = ();
    my @subents = ();
    while (<>) {
	chomp;
	my $eref = ${$acd{'ehash'}}{$_};
	if ($eref) {
	    push @subents, $_;
#	    push @sub, $$eref; # don't need this as long as we acd_sort right after editing ehash
	} else {
	    warn "$0: '$_' is not an entry in ".pp_file()."\n";
	}
    }

    my %subents = (); @subents{@subents} = ();
    my %ehash = %{$acd{'ehash'}};
    my @deletia = ();

    if ($inverted) {
	foreach my $e (keys %ehash) {
	    push(@deletia, $e) if exists $subents{$e};
	}
    } else {
	foreach my $e (keys %ehash) {
	    push(@deletia, $e) unless exists $subents{$e};
	}
    }

    foreach my $d (@deletia) {
	delete $ehash{$d};
    }

    %{$acd{'ehash'}} = %ehash;

    @{$acd{'entries'}} = ();
    pp_acd_sort(\%acd);
    pp_serialize($h,\%acd);
} else {
    pp_diagnostics();
}

1;
