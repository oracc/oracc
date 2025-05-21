#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $file = shift @ARGV;
my $tmp = $file;
$tmp =~ s/xml$/x/;
open(X, ">$tmp") || die "$0: unable to write $tmp\n";
open(F, $file) || die "$0: unable to read $file\n";
while (<F>) {
    s/([0-9])--([0-9])/${1}–${2}/g;
    s/---/—/g;
    print X;
}
close(F);
close(X);

system 'mv', $tmp, $file;

1;
