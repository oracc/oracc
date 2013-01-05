#!/usr/bin/perl
use warnings; use strict;
my %sdoll = ();
my $log = shift @ARGV;
my $atf = shift @ARGV;
die unless $log && $log =~ /\.log$/ && $atf =~ /\.atf$/;
open(L, $log) || die;
while (<L>) {
    if (/:(\d+):.*?\$-line/ && !/meets strict/) {
	++$sdoll{$1};
    }
}
close L;
open(A, $atf);
while (<A>) {
    if (/^\$/ && $sdoll{$.}) {
	s/^\$ (.*?)\s*$/\$ ($1)\n/;
    }
    print;
}

1;
