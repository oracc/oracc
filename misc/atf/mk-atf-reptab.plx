#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

while (<>) {
    if (/no FORM `(.*?)'.*?match(?:es)? for ([^\[]+\[.*?\]\S*)/) {
	print "$1\t$2\t\n";
    }
}

1;
