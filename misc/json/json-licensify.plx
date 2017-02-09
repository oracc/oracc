#!/usr/bin/perl
use lib "$ENV{'ORACC'}/lib";
use ORACC::JSON;
use warnings; use strict;

my $raw = 0;
if ($ARGV[0] eq '-r') {
    shift @ARGV;
    $raw = 1;
}

warn "json-licensify.plx: adding default metadata, validating and formatting JSON ...\n";
foreach my $f (sort @ARGV) {
    warn "$f\n";
    my @t = ();
    open(F,$f) || die "json-licensify.plx: strange, can't open $f\n";
    while (<F>) {
	push @t, $_;
	if (/\"project\"/) {
	    my $defaultprops = ORACC::JSON::default_metadata($_,1);
	    if ($defaultprops) {
		if (/,\s*$/) {
		    chomp($defaultprops);
		    $defaultprops .= ",\n";
		}
		push @t, $defaultprops;
		while (<F>) {
		    push @t;
		}
	    }
	}
    }
    close(F);
    if ($raw) {
	print @t;
    } else {
	open(F,"|jq .>$f"); print F @t; close(F);
    }
}
1;
