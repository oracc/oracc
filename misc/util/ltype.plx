#!/usr/bin/perl
use warnings; use strict;
undef $/;
warn("NR=newline/return (DOS); N=newline (Unix); R=return (Mac)\n");
while ($#ARGV >= 0) {
    my $fn = shift @ARGV;
    if (open(F,$fn)) {
	binmode(F,':bytes');
	my $contents = <F>;
	my $sep;
	if ($contents =~ /\n\r/) {
	    $sep = "NR";
	} elsif ($contents =~ /\n\r/) {
	    $sep = "RN (!bad DOS format)";
	} elsif ($contents =~ /\r/) {
	    $sep = "R";
	} elsif ($contents =~ /\n/) {
	    $sep = "N";
	} else {
	    $sep = "(none)";
	}
	print STDERR "$fn: $sep\n";
	close(F);
    } else {
	warn("$fn: open failed\n");
    }
}
1;
