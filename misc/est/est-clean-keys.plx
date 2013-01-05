#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

while (<>) {
    if (/^::/) {
	tr/:;,./    /;
	s/ +/ /g;
	1 while s/ 0/ /;
    }
    print;
}

1;

