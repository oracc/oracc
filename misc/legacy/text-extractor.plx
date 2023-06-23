#!/usr/bin/perl
use warnings; use strict;

system 'rm', '-fr', 'txt';
my @dirs = `find fix -type d`;
chomp @dirs;
foreach my $d (@dirs) {
    $d =~ s/fix/txt/;
    system 'mkdir', '-p', $d;
}

my @files = `find fix -type f`;
chomp @files;
foreach my $f (@files) {
    my $outfile = $f;
    $outfile =~ s/fix/txt/;
    $outfile =~ s/(?:fix|xml)$/txt/;
    my $xsl = 'tools/xml2txt.xsl';
    $xsl = '/home/oracc/lib/scripts/xml2txt.xsl'
	unless -r $xsl;
    warn "$f\n";
    system 'xsltproc', '--output', $outfile, $xsl, $f;
    clean_text($outfile);
}

##################################################################

sub
clean_par {
    1 while $_[0] =~ s/\}([bi])\@\@\1\{//;
    1 while $_[0] =~ s/}{//;
    1 while $_[0] =~ s/\@([bi])\{\s*\}\1\@//;
    print $_[0];
}

sub
clean_text {
    my $f = shift;
    open(F,$f);
    my @f = (<F>);
    close(F);
    open(F,">$f"); select F;
    foreach my $p (@f) {
	clean_par($p);
    }
    close(F);
}

1;
