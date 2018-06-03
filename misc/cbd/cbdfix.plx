#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Fix;
use ORACC::CBD::Hash;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

my %args = pp_args();

if ($args{'mode'}) {
    die "$0: -mode must be glossary or corpus\n"
	unless $args{'mode'} eq 'glossary' || $args{'mode'} eq 'corpus';
} else {
    die "$0: must give -mode=glossary or -mode=corpus on command line\n";
}

$ORACC::CBD::nonormify = 1;

my $h = '';

if (($h = pp_hash(\%args))) {
    pp_fix(\%args, $h);
} else {
    pp_diagnostics();
}

1;
