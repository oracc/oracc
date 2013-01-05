#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8'; binmode STDOUT, ':utf8';

while (<>) {
    s/(\\x\{.*?\})/&fix($1)/eg;
    print;
}

sub
fix {
    my $x = shift;
    $x =~ s/^\\x\{(.*?)\}$/$1/;
    my $char = eval("0x$x");
    chr($char);
}

1;
