#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

# Feed this a pqids.lst and it will recursively find all the proxies
# and write a new list with the mappings already resolved.

my %maps = ();

while (<>) {
    next if /^\s*$/ || /^\#/;
    chomp;
    my($prj,$pqx) = (/^(.*?):(.*?)$/);
    while (1) {
	load_pqids($prj) unless $maps{$prj};
	my $nprj = ${$maps{$prj}}{$pqx};
	if ($nprj) {
	    $prj = $nprj;
	} else {
	    last;
	}
    }
}

sub load_pqids {
    my $prj = shift;
    my $f = "$ENV{'ORACC_BUILDS'}/pub/$prj/cat/pqids.lst";
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

1;
