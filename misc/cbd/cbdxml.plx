#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::XML;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

my %args = pp_args();

my $x = pp_xml(\%args);

if ($x) {
    binmode STDOUT, ':raw';
    print $x->toString();
} else {
    pp_diagnostics();
}

1;
