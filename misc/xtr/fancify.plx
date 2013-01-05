#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDERR, ':utf8'; binmode STDOUT, ':utf8';

my $fancifying = 1;

while (<>) {
    if (/^\&/) {
	$fancifying = 0;
	print;
    } elsif (/^\@translation/) {
	if (/parallel/) {
	    $fancifying = 2;
	} else {
	    $fancifying = 1;
	}
	print;
    } elsif ($fancifying) {
	my $prefix = '';
	if ($fancifying == 2) {
	    s/^(\S+)\s+//;
	    $prefix = $1; 
	} elsif (/^\@(?:label)?\(/) {
	    s/^(.*?\)\s*)//;
	    $prefix = $1;
	}
	fancify($_);
	print $prefix, $_;
    } else {
	print;
    }
}

sub
fancify {
    s/([ivx\d])'/$1′/g;
    1 while s/′'/′′/;
    s/(^|[\[(\s])"/$1“/g;
    s/"([.;,\s\])]|$)/”$1/g;
    s/(^|[\[(\s])[`']/$1‘/g;
    s/'([.;,\s\])]|$)/’$1/g;
    s/(\d)--?(\d)/$1–$2/g;
    s/---?/—/g;
}

1;
