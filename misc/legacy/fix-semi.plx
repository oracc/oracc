#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
while (<>) {
    if (/^\@sense/) {
	tr/;/,/;
    }
    print;
}
1;
