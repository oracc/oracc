#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Fix;

my %args = pp_args(); $args{'mode'} = 'glossary'; $args{'stdout'} = 1;

$ORACC::CBD::nonormify = 1;

my @cbd = setup_cbd(\%args);

if (pp_status()) {
    pp_diagnostics();
    exit 1;
} else {
    pp_status(0);
}

my %f = pp_fix_load_fixes(\%args,0);

# use Data::Dumper; warn Dumper \%f; exit 1;

foreach my $i (pp_tags('entry',@cbd)) {
    my($cfgw) = ($cbd[$i] =~ /\s+(.*?)\s*$/);    
    if ($f{$cfgw}) {
	warn "mapping $cbd[$i] to $f{$cfgw}\n";
	$cbd[$i] = "\@entry $f{$cfgw}";
    }
}

pp_cbd(\%args,@cbd);

1;
