#!/usr/bin/perl -C0
use warnings; use strict;

while (<>) {
    my @toks = split;
    my $w = shift @toks;
    $w =~ tr/_/ /;
    print "$w\t";
    my $last_text = '';
    foreach my $t (@toks) {
	$t =~ tr/_/ /;
	my($x,$l) = ($t =~ /^(.*?): (.*?)$/);
	if ($x eq $last_text) {
	    print ", $l";
	} else {
	    print '; ' if $last_text;
	    $last_text = $x;
	    print "$x: $l";
	}
    }
    print ".\n";
}

1;
