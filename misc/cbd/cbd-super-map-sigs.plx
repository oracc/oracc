#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Map;

use Data::Dumper;

$ORACC::CBD::nosetupargs = 1;
my %args = pp_args();

my $map = shift @ARGV;
if (-r $map) {
    my %map = map_load(shift @ARGV,'sigs');
    while (<>) {
	if (/^\@[^%]+\s/ || /^\s*$/ || /^#/) {
	    print;
	} else {
	    print map_apply_sig($_);
	}
    }
} else {
    die "$0: must give map on command line\n";
} 

1;
