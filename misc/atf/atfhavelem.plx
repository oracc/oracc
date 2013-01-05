#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8';
my $P = undef;
my %have_lem = ();
while (<>) {
    if (/^.?&([PQX]\d+)/) {
	$P = $1 unless /^\#/;
    } elsif (/^\#lem:/) {
	++$have_lem{$P};
    }
}

print join("\n", sort keys %have_lem),"\n";

1;
