#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

while (<>) {
    if (/^\@form/) {
	s/\s\+-+/ +/;
	s/\s\+\.?(.*?)=[\+\.]*(.*?)($|\s)/ +-$1=$2$3/;
    }
    print;
}

1;
