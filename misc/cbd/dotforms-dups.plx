#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $cfgw = '';
my %forms = ();
my %seen = ();
my $file = shift @ARGV;
die unless $file;
open(F,$file) || die;
while (<F>) {
    my($thiscfgw,$form) = (/^(.*?)\t(.*?)$/);
    my($orth) = ($form =~ /^\@form\s+(\S+)/);
    if ($thiscfgw ne $cfgw) {
	$cfgw = $thiscfgw;
	%seen = ();
	++$seen{$orth};
    } else {
	if ($seen{$orth}++) {
	    warn "$file:$.: duplicate orth\n";
	}
    }
}

1;
