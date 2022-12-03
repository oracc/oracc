#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $cfgw = '';
my %forms = ();
my %seen = ();
my $proj = 'epsd2';
my $lang = 'sux';
my $file = shift @ARGV;
my @input = ();
if ($file) {
    die unless -r $file;
    @input = `cat $file`;
} else {
    @input = (<>);
}

my $line = 0;

foreach (@input) {
    chomp;
    ++$line;
    my $norm;
    my($cfgw,$form,$lxng,$base,$cont,$morph)
	= (m#^(.*?)\t\@form\s+(\S+)(\s+\%\S+)?\s+/(\S+)(\s+\+\S+)?\s+(\S+)\s*$#);
    unless ($cfgw) {
	($cfgw,$form,$lxng,$norm)
	    = (m#^(.*?)\t\@form\s+(\S+)(\s+\%\S+)?\s+\$(\S+)\s*$#);
    }
    $lang = $lxng if $lxng;
    if ($cfgw) {
	$cfgw =~ s/\s+(\[.*?\])\s+/$1/;
	$cfgw =~ s#\[(.*?)\]#[$1//$1]#;
	$cfgw =~ s/\](\S+)/\]$1'$1/;
	$cont = '' unless $cont; $cont =~ s/^\s*//;
	$lang =~ s/^\s*// if $lang;
	$lang = "\%$lang" unless $lang =~ /^\%/;
	if ($base) {
	    print "\@$proj$lang:$form=$cfgw/$base$cont$morph\n";
	} else {
	    print "\@$proj$lang:$form=$cfgw\$$norm\n";
	}
    } else {
	warn "$line: bad syntax in forms line\n";
    }
}

1;
