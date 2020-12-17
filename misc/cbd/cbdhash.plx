#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Hash;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

my %args = pp_args();
my $hash = undef;
if (($hash = pp_hash(\%args))) {
    my $cbdname = ${$ORACC::CBD::data{'cbds'}}[0];
    if ($args{'output'}) {
	open(O,">$args{'output'}") || die "$0: can't save output to $args{'output'}\n";
	select O;
	my %acd = ORACC::CBD::Hash::pp_hash_acd($hash);
	ORACC::CBD::Hash::pp_serialize($hash,\%acd);
	close(O);
    } else {
	use Data::Dumper;
	print Dumper \%{$ORACC::CBD::data{$cbdname}};
    }
} else {
    pp_diagnostics();
}

1;
