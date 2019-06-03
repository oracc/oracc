#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::OID;

my %args = pp_args();
$args{'stdout'} = 1;
my @c = setup_cbd(\%args);
warn "adding oids ...\n";
@c = oid_add(\%args, @c);
warn "done adding oids.\n";
pp_diagnostics(\%args);
pp_cbd(\%args, @c);

1;
