#!/usr/bin/perl
use warnings; use strict;

while (<>) {
    if (/^([^,]+)_/) {
	s/^(.*?)_/$1,/;
    }
    print;
}

1;
