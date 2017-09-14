#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $file = shift @ARGV;
my $tmp = $file;
$tmp =~ s/xml$/x/;
open(X, ">$tmp");
open(F, $file);
while (<F>) {
    s/([0-9])--([0-9])/${1}–${2}/g;
    s/---/—/g;
    print X;
}
close(F);
close(X);

system 'mv', $tmp, $file;

1;
