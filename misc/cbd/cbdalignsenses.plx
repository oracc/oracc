#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use Data::Dumper;

use ORACC::CBD::XML;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Senses;

my %args = pp_args();
my %base_senses = ();
my @base_cbd = ();
my $curr_entry = '';

if ($args{'base'}) {
    @base_cbd = setup_cbd(\%args,$args{'base'});
}
my @cbd = setup_cbd(\%args);

senses_align(\%args, \@base_cbd, \@cbd);

pp_diagnostics() if pp_status();

1;
