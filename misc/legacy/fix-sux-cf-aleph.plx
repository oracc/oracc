#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $cf = '';
my $pos = '';

while (<>) {
    if (/^\@entry/) {
	s/(\s)(\S+)/fix_aleph($1,$2)/e;
    }
    print;
}

sub
fix_aleph {
    my($ws,$cf) = @_;
    $cf =~ tr/'/ʾ/;
    "$ws$cf";
}

1;
