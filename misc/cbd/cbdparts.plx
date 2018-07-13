#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my %parts = ();
while (<>) {
    if (/^\@end\s+entry/) {
	%parts = ();
	print;
    } elsif (/^\@parts/) {
	s/\s+/ /g;
	s/\s*$/\n/;
	print unless $parts{$_}++;
    } else {
	print;
    }
}


1;
