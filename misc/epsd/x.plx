#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;

binmode STDOUT, ':utf8';

print utfify("d7"), "\n";
print utfify("2083"), "\n";

sub
utfify {
    chr(hex("0x$_[0]"));
}

1;
