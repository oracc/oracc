#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $cfgw = '';
my %forms = ();
my %seen = ();
my $proj = 'epsd2';
my $lang = 'sux';
my $file = shift @ARGV;
die unless $file;
open(F,$file) || die;
while (<F>) {
    chomp;
    my($cfgw,$form,$base,$cont,$morph) = (m#^(.*)\t\@form\s+(\S+)\s+/(\S+)(\s+\+\S+)?\s+(\S+)\s*$#);
    $cfgw =~ s/\s+(\[.*?\])\s+/$1/;
    $cfgw =~ s#\[(.*?)\]#[$1//$1]#;
    $cfgw =~ s/\](\S+)/\]$1'$1/;
    $cont = '' unless $cont; $cont =~ s/^\s*//;
    print "\@$proj\%$lang:$form=$cfgw/$base$cont$morph\n";
}

1;
