#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use Data::Dumper;
use ORACC::CBD::Util;
use ORACC::CBD::PPWarn;
use ORACC::CBD::History;

my %args = pp_args();

if ($args{'entries'}) {
    my $g = $args{'entries'};
    Encode::_utf8_on($g);
    if ($args{'all'}) {
	history_all_init();
    } else {
	history_etc_init();
    }

    history_dumper() if $args{'trace'};
	
    if ($g =~ m#//#) {
	my $m = history_guess_sense($g);
	print "$g => $m\n";
    } else {
	my $m = history_guess($g);
	print "$g => $m\n";
    }
} elsif ($args{'oids'}) {
    if ($args{'all'}) {
	history_all_init();
    } else {
	history_etc_init();
    }
    history_oid();    
} else {
    history_trim();
}

1;
