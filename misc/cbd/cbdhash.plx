#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Hash;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

my %args = pp_args();

if (pp_hash(\%args)) {
    my $cbdname = ${$ORACC::CBD::data{'cbds'}}[0];
    use Data::Dumper;
    print Dumper \%{$ORACC::CBD::data{$cbdname}};
} else {
    pp_diagnostics();
}

1;
