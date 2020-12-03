package ORACC::CBD::SLStats;

require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/slstats_load slstats_guess_cfgw/;

$ORACC::CBD::SLStats::verbose = 0;

use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

# Module for managing/querying statistics output from slsig-stats.plx

my %baresig = ();
my %cfgsig = ();
my %forms = ();

sub slstats_load {
    my @projects = @_;
    foreach my $p (@projects) {
	if (system "$ENV{'ORACC_BUILDS'}/bin/slsig-stats.plx", '-p', $p) {
	    warn "$0: failure loading stats from project $p\n";
	    return 1;
	}
    }
    
    foreach my $p (@projects) {
	load_stats_file($p);
    }

    return 0;
}

# Receive a sign signature and return a list of structure:
# [ FOUND_TLIT , FOUND_CFGW , FOUND_FORM ] , ...
sub slstats_guess_cfgw {
    my $s = shift;
    my $cfg = $b;
    my $orth = '';
    my $form = '';
    my @res = ();
    if (($cfg = $baresig{$s})) {
	my @cfg = split(/\t/, $cfg);
	warn "$s => $baresig{$s}\n" if $ORACC::CBD::SLStats::verbose;
	foreach my $c (@cfg) {
	    $c =~ s/<.*>$//;
	    my $orthcfg = "$s\::$c";
	    if (($orth = $cfgsig{$orthcfg})) {
		warn "$orthcfg => $orth\n" if $ORACC::CBD::SLStats::verbose;
		$orth =~ s/^=//; $orth =~ s/<.*?>$//;
		my $formkey = "$c\::=$s";
		if (($form = $forms{$formkey})) {
		    warn "$formkey => $form\n" if $ORACC::CBD::SLStats::verbose;
		    push @res, [ $orth , $c , $form ];
		} else {
		    warn "$formkey not found in forms hash\n" if $ORACC::CBD::SLStats::verbose;
		}
	    } else {
		warn "$orthcfg not found in cfgsig hash\n" if $ORACC::CBD::SLStats::verbose;
	    }
	}
    }
    @res;
}

sub load_stats_file {
    my $f = "$ENV{'ORACC_BUILDS'}/$_[0]/01bld/project.stats";
    open(F,$f) || die "$0: unable to load stats file $f\n";
    warn "$0: loading stats file $f\n";
    while (<F>) {
	chomp;
	my($k,$v) = split(/\t/,$_);
	if ($k =~ /::=/) {
	    $forms{$k} = $v unless $forms{$k};
	} elsif ($v =~ m#^[/=]#) {
	    $cfgsig{$k} = $v unless $cfgsig{$k};
	} else {
	    $baresig{$k} = $v unless $baresig{$k};
	}
    }
    close(F);
    # use Data::Dumper; print Dumper \%forms;
}

1;
