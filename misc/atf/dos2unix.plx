#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

undef $/;

foreach my $a (@ARGV) {
    open(A,$a) || die;
    my $text = <A>;
    close(A);
    $text =~ tr/\r//d;
    open(A,">new/$a") || die;
    print A $text;
    close(A);
}

1;
