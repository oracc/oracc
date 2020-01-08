#!/usr/bin/perl
use warnings; use strict; use open ':utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

# Create a version of the ATF input that doesn't have any flags and brackets,
# suitable for grepping, but not necessarily fit for reparsing as ATF because
# it doesn't consider the context of brackets.

while (<>) {
    unless (/^\#lem:/) {
	tr/[]<>#*?!//d;
    }
    print;
}

1;
