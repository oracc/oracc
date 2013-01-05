#!/usr/bin/perl
use warnings; use strict;

system 'rm', '-fr', 'convert/fix';
my @dirs = `find convert/xml -type d`;
chomp @dirs;
foreach my $d (@dirs) {
    $d =~ s/xml/fix/;
    system 'mkdir', '-p', $d;
}

my @files = `find convert/xml/ -type f`;
chomp @files;
foreach my $f (@files) {
    my $outfile = $f;
    $outfile =~ s/xml/fix/;
    $outfile =~ s/xml$/fix/;
    system "/home/cdl/legacy/tools/fixchar.plx", '-out', $outfile, $f;
}

1;
