#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $arg = shift @ARGV;
while (<>) {
    if ($arg =~ /^sux/ || ($arg =~ /^qpn/ && /\%sux/)) {
	s/\s\$\S+//g;
    }
    print;
}

1;
