#!/usr/bin/perl
use warnings; use strict; use open ':utf8'; use utf8;
binmode STDIN, 'utf8'; binmode STDOUT, 'utf8'; binmode STDERR, 'utf8';

while (<>) {
    if (s/^\#lem:\s*//) {
	chomp;
	my @l = split(/;\s+/, $_);
	my @nl = ();
	foreach my $l (@l) {
	    if ($l =~ m#\].*?/#) {
		push @nl, fix($l);
	    } else {
		push @nl, $l;
	    }
	}
	chomp @nl;
	print "#lem: ", join('; ', @nl), "\n";
    } else {
	print;
    }
}

sub fix {
    my $x = $_[0];
    $x =~ s/\+//;
    $x =~ s/].*$/]/;
    warn "$_[0] => $x\n";
    $x;
}

1;
