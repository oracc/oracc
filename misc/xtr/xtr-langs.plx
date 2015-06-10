#!/usr/bin/perl
use warnings; use strict;
my %langs = ();
my @xtf_files = `cat 01bld/lists/have-xtf.lst 01bld/lists/proxy-atf.lst | pqxpand xtf`;
foreach my $x (@xtf_files) {
    chomp $x;
    $x =~ s/\.xtf/*.xtr/;
    my @g = glob $x;
    foreach (@g) {
	/-(.*?)\.xtr$/;
	++$langs{$1};
    }
}

print join("\n", sort keys %langs), "\n";

1;
