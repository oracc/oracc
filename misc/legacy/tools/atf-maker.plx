#!/usr/bin/perl -C0
use warnings; use strict;
my $nth = 0;
die "atf-maker.plx: I only work when there is an executable tools/txt2atf.plx\n"
    unless -x "tools/txt2atf.plx";

system 'rm', '-fr', 'convert/atf';
my @dirs = `find convert/txt -type d`;
chomp @dirs;
foreach my $d (@dirs) {
    $d =~ s/txt/atf/;
    system 'mkdir', '-p', $d;
}

my @files = `find convert/txt -type f`;
chomp @files;
foreach my $f (@files) {
    my $outfile = $f;
    $outfile =~ s/txt/atf/;
    $outfile =~ s/txt$/atf/;
    system "tools/txt2atf.plx -file '$f' -nth=$nth <'$f' >'$outfile'";
    ++$nth;
}

1;
