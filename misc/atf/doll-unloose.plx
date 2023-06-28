#!/usr/bin/perl
use warnings; use strict;
my %sdoll = ();
my $log = shift @ARGV;
my $atf = shift @ARGV;
die unless $log && $log =~ /\.log$/ && $atf =~ /\.atf$/;
open(L, $log) || die;
while (<L>) {
    if (/:(\d+):/) {
	my $ln = $1;
	++$sdoll{$ln} if /\$-line meets strict/;
    }
}
close L;
open(A, $atf);
while (<A>) {
    if (/^\$/ && $sdoll{$.}) {
	s/^\$\s*\((.*?)\)\s*$/\$ $1\n/ || warn("bad sub at $.\n");
    }
    print;
}

1;
