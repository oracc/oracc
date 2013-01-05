#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

while (<>) {
    if (/^\@sense/) {
	s/(\s)"/$1\x{201c}/;
	s/"(\s)/\x{201d}$1/;
	tr/\[\]/\x{27e6}\x{27e7}/;
    }
    print;
}

1;
