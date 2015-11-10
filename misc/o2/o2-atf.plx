#!/usr/bin/perl
use warnings; use strict;
my $oracc = $ENV{'ORACC'};
my $bin = "$oracc/bin";
my $O2ARGS = '';
my $ox = "$bin/ox";
my $project = `oraccopt`;
my $verbose = 1;

sub xsystem;

# run atfsplit and update installation
if (-s '01bld/atfsources.lst') {
    atf_options();
    warn("o2-atf.plx: updating ATF installation in project $project\n")
	if $verbose;
    xsystem 'rm', '-f', "01bld/lists/valid.lst", "01bld/lists/invalid.lst";
    xsystem "$ox -I01bld/atfsources.lst -Cq${O2ARGS} -P$project -b01bld/lists/invalid.lst -g01bld/lists/valid.lst | xmlsplit -u -c01bld/lists/changed.lst";
}

sub
atf_options {
    my $atf_flags = `oraccopt $project atf-flags`;
    my $atf_lem_psa = `oraccopt $project atf-lem-psa`;
    my $atf_lem_extended = `oraccopt $project atf-lem-extended`;
    my $views_cuneify = `oraccopt $project views-cuneify`;
    if ($atf_lem_psa) {
	if ($atf_lem_psa eq 'properties') {
	    $O2ARGS .= 'y';
	} elsif ($atf_lem_psa eq 'entities') {
	    $O2ARGS .= 'Y';
	} elsif ($atf_lem_psa eq 'none') {
	} else {
	    die "o2-atf.plx: value $atf_lem_psa' not known for 'atf-lem-psa'\n";
	}
    } elsif ($atf_lem_extended) {
	if ($atf_lem_extended eq 'yes') {
	    $O2ARGS .= 'D';
	} elsif ($atf_lem_extended eq 'no') {
	} else {
	    die "o2-atf.plx: value '$atf_lem_extended' not known for 'atf-lem-extended'\n";
	}
    } elsif ($atf_flags) {
	$O2ARGS .= $atf_flags;
    }
    if ($views_cuneify && $views_cuneify =~ /tru|yes/) {
	$O2ARGS .= ' -w';
    }
}

sub
xsystem {
    warn "system @_\n" if $verbose;
    system @_;
}

1;
