#!/usr/bin/perl -p
use warnings; use strict; use open 'utf8'; use utf8;

binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

s/([₀-₉]+\()/updig($1)/eg;

s,([₁₂₃₄₅]/[0-9]),updig($1),eg;

sub
updig {
    my $x = shift;
    $x =~ tr/₀-₉/0-9/;
    $x;
}

1;
