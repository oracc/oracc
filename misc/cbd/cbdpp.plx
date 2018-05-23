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

my %args = pp_args();

my @cbd = setup_cbd(\%args);

if (pp_status() && !$args{'force'}) {
    my $ret = pp_diagnostics(\%args);
    system 'touch', '01bld/cancel';
    die("cbdpp.plx: $ret errors in glossary $args{'cbd'}. Stop.\n");
} else {
    
    if ($args{'edit'}) {
	@cbd = edit(\%args, @cbd);
	pp_diagnostics(\%args);
	exit 1 if pp_status();
    }

    unless ($args{'check'} || $args{'sigs'}) {
	foreach my $f (keys %ppfunc) {
	    if ($#{$ORACC::CBD::Util::data{$f}} >= 0) {
		pp_trace("cbdpp/calling ppfunc $f");
		&{$ppfunc{$f}}(\%args, $f);
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
    sigs_from_glo(\%args) unless $args{'check'} || (pp_status() && !$args{'force'});
    pp_trace("cbdpp/cbd write complete");
}

pp_diagnostics(\%args);

################################################
#
# CBDPP Operational Functions
#

sub pp_collo {
    my $args = shift;
    my $ndir = "$$args{'projdir'}/02pub";
    system 'mkdir', '-p', $ndir;
    open(COLLO, ">$ndir/coll-$$args{'lang'}.ngm");
    foreach my $i (@{$ORACC::CBD::Util::data{'collo'}}) {
	my $e = pp_entry_of($i,@cbd);
	my $c = $cbd[$e];
	$c =~ s/^\S*//;
	$c =~ s/\].*$/\]/;
	$c =~ s/\s+\[/\[/;
	my $cc = $cbd[$i];
	$cc =~ s/\s+-(\s+|$)/ $c /;
	$cc =~ s/\s+$//;
	$cc =~ s/^\S+\s+//;
	print COLLO $cc, "\n";
	$cbd[$i] = "\000";
    }
    close(COLLO);
}

sub pp_geo {
    my $geo = `oraccopt $args{'project'} cbd-geonames`;
    if ($geo && $geo ne 'no') {
	@cbd = geonames($geo,$ORACC::CBD::Util::data{'geo'}, @cbd);
    }
}

sub pp_usage {
    open(USAGE,'>pp.usage');
    foreach my $i (@{$ORACC::CBD::Util::data{'collo'}}) {
	print USAGE $cbd[$i], "\n";
	$cbd[$i] = "\000";
    }
    close(USAGE);
}

sub pp_zero {
    my ($args_ref,$func) = @_;
    foreach my $i (@{$ORACC::CBD::Util::data{$func}}) {
	$cbd[$i] = "\000";
    }
}

1;
