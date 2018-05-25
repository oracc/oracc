#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

while (<>) {
    chomp;
    if (/\(words\)\s+(.*?)\s+looks like\s+(.*?)\s*$/) {
	print "$1 => $2\n";
    } elsif (/'(.*?)'\s+(?:homograph|may be).*?'(.*?)' in base$/) {
	print "$1 => $2\n";
    } else {
	warn "no replacements found in $_\n";
    }
}

1;
