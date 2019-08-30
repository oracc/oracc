#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Util;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Edit;
use ORACC::CBD::Senses;

my %args = pp_args();

$args{'cbd'} = shift @ARGV unless $args{'cbd'};

if (-r $args{'cbd'}) {
    unless ($args{'output'}) {
	if ($args{'cbd'} =~ /aligned/) {
	    if ($args{'cbd'} =~ /entries|senses|bases|/) {
		$args{'output'} = $args{'cbd'};
		$args{'output'} =~ s/aligned/edited/;
		die "$0: unable to write to $args{'output'} to save edits. Stop\n"
		    unless -w ".";
	    }
	}
    }
    $args{'stdout'} = 1 unless $args{'output'};
} else {
    die "$0: unable to read $args{'cbd'} for edit. Stop.\n";
}

$ORACC::CBD::nonormify = 1;

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
    if ($args{'cbd'} =~ /senses/) {
	@cbd = senses_uniq(@cbd);
    }
    pp_cbd(\%args,@cbd);
    pp_diagnostics(\%args) if pp_status();
    
}

1;
