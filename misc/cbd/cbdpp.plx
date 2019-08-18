#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";

use ORACC::L2GLO::Langcore;
use ORACC::CBD::Util;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Edit;
use ORACC::CBD::Forms;
use ORACC::CBD::Geonames;
use ORACC::CBD::Collo;
use ORACC::CBD::Sigs;
use ORACC::CBD::XML;

# bare: no need for a header
# check: only do validation
# kompounds: check compounds (like |SI.A|) against OGSL
# dry: no output files
# edit: edit cbd via acd marks and write patch script
# filter: read from STDIN, write CBD result to STDOUT
# force: generate output CBD even if there are errors
# reset: reset cached glo and edit anyway 
# trace: print trace messages 
# vfields: only validate named fields, plus some essential ones supplied automatically

my %ppfunc = (
    usage=>\&pp_usage,
    collo=>\&pp_collo,
    proplist=>\&pp_zero,
    geo=>\&pp_geo,
);

#    sense=>\&pp_geo,

unlink '01bld/cancel';

my %args = pp_args();

if ($args{'edit'}) {
    die "cbdpp.plx: -force is not allowed with -edit. Stop.\n"
	if $args{'force'};
    $ORACC::CBD::nonormify = 1;
}

my @cbd = setup_cbd(\%args);

if ($args{'announce'}) {
    my $whatting = 'updating';
    if ($args{'check'}) {
	$whatting = 'checking';
    } elsif ($args{'sigs'}) {
	$whatting = 'getting signatures from';
    }
    warn "cbdpp.plx: $whatting $args{'cbd'}\n";
}

if (pp_status() && !$args{'force'}) {
    my $ret = pp_diagnostics(\%args);
    system 'touch', '01bld/cancel';
    my $e = errfile();
    if ($e ne $args{'cbd'}) {
	die("cbdpp.plx: $ret errors in $e (source of $args{'cbd'}). Stop.\n");
    } else {
	die("cbdpp.plx: $ret errors in $e. Stop.\n");
    }
} else {

    if ($args{'edit'}) {
	@cbd = edit(\%args, @cbd);
	warn "edit() returned $#cbd lines\n";
	pp_diagnostics(\%args);
#	exit 1 if pp_status();
	pp_cbd(\%args,@cbd);
	exit 0;
    }

    unless ($args{'check'} || $args{'sigs'}) {
	foreach my $f (keys %ppfunc) {
	    if ($#{$ORACC::CBD::data{$f}} >= 0) {
		pp_trace("cbdpp/calling ppfunc $f");
		&{$ppfunc{$f}}(\%args, $f, @cbd);
		pp_trace("cbdpp/exited ppfunc $f");
	    }
	}
    }
}

if ($args{'xml'}) {
    my $x = pp_xml(\%args,@cbd);
    if ($x) {
	binmode STDOUT, ':raw';
	print $x->toString();
    }
} else {
    pp_trace("cbdpp/writing cbd");
    pp_cbd(\%args,@cbd) unless $args{'check'} || $args{'sigs'};
#    forms_dump('forms-78.dump');
    sigs_from_glo(\%args, @cbd) unless $args{'nosigs'} || $args{'check'} || (pp_status() && !$args{'force'});
    pp_trace("cbdpp/cbd write complete");
}

pp_diagnostics(\%args);

################################################
#
# CBDPP Operational Functions
#

sub pp_geo {
    my $geo = `oraccopt . cbd-geonames`; #  $args{'project'}
    if ($geo && $geo ne 'no') {
	@cbd = geonames($geo,$ORACC::CBD::data{'geo'}, @cbd);
    }
}

sub pp_usage {
    open(USAGE,'>pp.usage');
    foreach my $i (@{$ORACC::CBD::data{'collo'}}) {
	print USAGE $cbd[$i], "\n";
	$cbd[$i] = "\000";
    }
    close(USAGE);
}

sub pp_zero {
    my ($args_ref,$func) = @_;
    foreach my $i (@{$ORACC::CBD::data{$func}}) {
	$cbd[$i] = "\000";
    }
}

1;
