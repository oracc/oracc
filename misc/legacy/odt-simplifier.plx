#!/usr/bin/perl
use warnings; use strict;

my $xsl = 'tools/odt2xml.xsl';
$xsl = '/Users/stinney/orc/lib/scripts/odt2xml.xsl'
    unless -r $xsl;

die "odt-simplifier.plx: no XSL script $xsl\n" unless -r $xsl;

system 'rm', '-fr', 'xml';
my @dirs = `find odt -type d`;
chomp @dirs;
foreach my $d (@dirs) {
    $d =~ s/odt/xml/;
    system 'mkdir', '-p', $d;
}

my @files = `find odt -type f`;
chomp @files;
foreach my $f (@files) {
    my $outfile = $f;
    $outfile =~ s/odt/xml/;
    $outfile =~ s/odt$/xml/;
    system 'xsltproc', '--output', $outfile, $xsl, $f;
}

1;
