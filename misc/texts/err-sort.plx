#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my @l = (<>);

print sort { &errcmp; } @l;

sub errcmp {
    my($af,$al) = ($a =~ /^(.*?):(.*?):/);
    my($bf,$bl) = ($b =~ /^(.*?):(.*?):/);
    $af cmp $bf || $al <=> $bl;
}

1;
