#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Bases;
use ORACC::CBD::Sort;

my %args = pp_args();

$args{'letters'} = 1;
$ORACC::CBD::noforms = 1;
$ORACC::CBD::nonormify = 1;

my @cbd = setup_cbd(\%args);

if ($args{'bases'}) {
    my $cpd = 0;
    for (my $i = 0; $i < $#cbd; ++$i) {
	if ($cbd[$i] =~ /^\@bases/) {
	    my $b = bases_serialize(bases_hash($cbd[$i],$cpd,$i+1));
	    $cbd[$i] = "\@bases $b";
	    $cpd = 0;
	} elsif ($cbd[$i] =~ /\@parts/) {
	    $cpd = 1;
	}
    }
}

if (pp_status()) {
    pp_diagnostics();
} else {
    pp_cbd(\%args, sort_cbd(\%args, @cbd));
}

1;
