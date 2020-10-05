#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

# Take a project as argument and recursively find all the proxies in the pqid list
# then rewrite the list with the mappings already resolved.

my $project = shift @ARGV; die "$0: must give project on command line\n" unless $project;
my $pqids = pqid_of($project); die "$0: no such file $pqids\n" unless -r $pqids;
my @pqids = `cat $pqids`; chomp @pqids;
open(N,">$pqids") || die "$0: unable to open $pqids to write new version\n";

my %maps = ();

foreach (@pqids) {
    next if /^\s*$/ || /^\#/;
    my($prj,$pqx) = (/^(.*?):(.*?)$/);
    my %seen = (); my @seen = ();
    while (1) {
	load_pqids($prj) unless $maps{$prj};
	my $nprj = ${$maps{$prj}}{$pqx};
	if ($nprj && $seen{$nprj}) { # detect circular proxy
	    warn "$0: project $project\:$pqx has circular proxy $nprj (@seen)\n";
	    last;
	}
	if ($nprj && $nprj ne $prj) {
	    $prj = $nprj;
	    push @seen, $nprj;
	    ++$seen{$nprj};
	} else {
	    last;
	}
    }
    print N "$prj:$pqx\n";
}

sub load_pqids {
    my $prj = shift;
    my $f = pqid_of($prj);
    if (open(F,$f)) {
	my %m = ();
	while (<F>) {
	    chomp;
	    /^(.*?):(.*?)$/;
	    $m{$2} = $1;
	}
	close(F);
	%{$maps{$prj}} = %m;
    } else {
	warn "$0: no file $f\n";
	%{$maps{$prj}} = ();
    }
}

sub pqid_of {
    "$ENV{'ORACC_BUILDS'}/pub/$_[0]/cat/pqids.lst";
}

1;
