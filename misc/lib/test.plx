#!/usr/bin/perl
use warnings; use strict;

test("wanker");
test("wanker pm");
test(". pm");
test(". px");

sub
test {
    my $t = `condfind.plx $_[0]`;
#    my $t = !$?;
    if ($t) {
	warn "ok\n";
    } else {
	warn "nope\n";
    }
}

1;
