#!/usr/bin/perl
use warnings; use strict;
use Getopt::Long;
use Time::HiRes;
my $oracc = $ENV{'ORACC'};
my $batch = 0;
my $bin = "$oracc/bin";
my $O2ARGS = '';
my $ox = "$bin/ox";
my $project = `oraccopt`;
my $verbose = 1;

GetOptions(
    'batch'=>\$batch,
    );

sub xsystem;

# run atfsplit and update installation
if ($batch) {
    atf_options();
    warn("o2-atf.plx: batch updating ATF installation in project $project\n")
	    if $verbose;
    foreach my $a (<01bld/atf-batch-*.lst>) {
	my $b = $a; $b =~ s/.*?-(\d\d\d)\.lst/$1/;
	my $log = $a; $log =~ s/bld/tmp/; $log =~ s/st$/og/;
	xsystem 'rm', '-f', "01bld/lists/valid-$b.lst", "01bld/lists/invalid-$b.lst";
	my $start_time = [Time::HiRes::gettimeofday()];
	warn "o2-atf-batch.plx: start batch $b\n";
	xsystem "$ox -I$a -C${O2ARGS} -P$project -b01bld/lists/invalid-$b.lst -g01bld/lists/valid-$b.lst  -l$log| xmlsplit -f -u -c01bld/lists/changed-$b.lst";
	my $diff = Time::HiRes::tv_interval($start_time);
	warn "o2-atf-batch.plx: batch $b took $diff seconds\n";
    }
} else {
    if (-s '01bld/atfsources.lst') {
	atf_options();
	warn("o2-atf-batch.plx: updating ATF installation in project $project\n")
	    if $verbose;
	xsystem 'rm', '-f', "01bld/lists/valid.lst", "01bld/lists/invalid.lst";
	xsystem "$ox -I01bld/atfsources.lst -C${O2ARGS} -P$project -b01bld/lists/invalid.lst -g01bld/lists/valid.lst | xmlsplit -f -u -c01bld/lists/changed.lst";
    }
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
    } 
    if ($atf_lem_extended) {
	if ($atf_lem_extended eq 'yes') {
	    $O2ARGS .= 'D';
	} elsif ($atf_lem_extended eq 'no') {
	} else {
	    die "o2-atf.plx: value '$atf_lem_extended' not known for 'atf-lem-extended'\n";
	}
    }
    if ($atf_flags) {
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
