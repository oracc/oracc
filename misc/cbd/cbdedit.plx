#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Util;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Edit;

my %args = pp_args();
$args{'stdout'} = 1;

if ($args{'edit'}) {
    die "cbdpp.plx: -force is not allowed with -edit. Stop.\n"
	if $args{'force'};
    $ORACC::CBD::nonormify = 1;
}

my @cbd = setup_cbd(\%args);

if (pp_status()) {

    my $ret = pp_diagnostics(\%args);
    system 'touch', '01bld/cancel';
    my $e = errfile();
    if ($e ne $args{'cbd'}) {
	die("cbdpp.plx: $ret errors in $e (source of $args{'cbd'}). Stop.\n");
    } else {
	die("cbdpp.plx: $ret errors in $e. Stop.\n");
    }

} else {

    @cbd = edit(\%args, @cbd);
#    warn "edit() returned $#cbd lines\n";
    pp_cbd(\%args,@cbd);
    pp_diagnostics(\%args);
    
}

1;
