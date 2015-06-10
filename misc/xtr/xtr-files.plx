#!/usr/bin/perl
use warnings; use strict;

my @xtf_files = `cat 01bld/lists/have-xtf.lst 01bld/lists/proxy-atf.lst | pqxpand xtf`;
foreach my $x (@xtf_files) {
    chomp $x;
    $x =~ s/\.xtf/*.xtr/;
    my @g = glob $x;
    if ($#g >= 0) {
	print join("\n", @g), "\n";
    }
}

1;
