#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

while (<>) {
    1 while s/([A-Z₀-₉][\[\]<>\?\!\*\#]*)\{(.*?)\}/phondet($1,$2)/g;
    tr/\000/{/;
    print;
}

sub
phondet {
    
}

1;
