#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::L2GLO::Util;

while (<>) {
    /^\{/ || next;
    chomp;
    s/\}::(.*)$// || next;
    my $subsigs = $1;
    my($form,$cfgw) = (m/^\{(.*?) = .*? \+= (.*?)$/);
    $form =~ tr/ /_/;
    $cfgw =~ s#//.*?\]#]#;
    $cfgw =~ s/'.*$//;
    print "$cfgw\t\@form $form\n";
    foreach my $s (split(/\+\+/,$subsigs)) {
	my %s = parse_sig($s);
	my $s = "\@form\t$s{'form'} /$s{'base'} ";
	if ($s{'cont'}) {
	    $s .= "+$s{'base'} ";
	}
	$s .= $s{'morph'};
	if ($s{'morph2'}) {
	    $s .= " $s{'morph2'}";
	}
	print "$cfgw\t$s\n";
    }
}

1;
