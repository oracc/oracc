#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Hash;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

my %args = pp_args();
my $h = undef;
if (($h = pp_hash(\%args))) {
    print join("\n", pp_hash_cfgws($h)), "\n";
} else {
    pp_diagnostics();
}

1;
