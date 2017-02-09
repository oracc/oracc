#!/usr/bin/perl
use lib "$ENV{'ORACC'}/lib";
use ORACC::JSON;
use warnings; use strict;

foreach my $f (@ARGV) {
    my @t = ();
    open(F,$f) || die "json-licensify.plx: strange, can't open $f\n";
    while (<F>) {
	push @t, $_;
	if (/\"project\"/) {
	    my $defaultprops = ORACC::JSON::default_metadata($_,1);
	    if (/,\s*$/) {
		chomp($defaultprops);
		$defaultprops .= ",\n";
	    }
	    push @t, $defaultprops;
	}
    }
    print @t;
    close(F);
}
1;
