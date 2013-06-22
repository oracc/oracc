#!/usr/bin/perl
use warnings; use strict;

my $xsl = 'tools/odt2xml.xsl';
$xsl = '/Users/stinney/oracc/misc/legacy/tools/odt2xml.xsl'
    unless -r $xsl;

die "odt-simplifier.plx: no XSL script $xsl\n" unless -r $xsl;

system 'rm', '-fr', 'convert/xml';
my @dirs = `find convert/odt -type d`;
chomp @dirs;
foreach my $d (@dirs) {
    $d =~ s/odt/xml/;
    system 'mkdir', '-p', $d;
}

my @files = `find convert/odt/ -type f`;
chomp @files;
foreach my $f (@files) {
    my $outfile = $f;
    $outfile =~ s/odt/xml/;
    $outfile =~ s/odt$/xml/;
    system 'xsltproc', '--output', $outfile, $xsl, $f;
}

1;
