#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

while (<>) {
    s/(\|[^\|]+?x[\(\[⸢]?[34A-ZŊŠ][^\|]*\|)/map_x($1)/eg;
    print;
}

sub map_x { my $t = shift; $t =~ tr/x/×/; $t; }

1;
