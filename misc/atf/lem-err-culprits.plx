#!/usr/bin/perl
use warnings; use strict; use utf8; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

while (<>) {
    if (/FORM|match/) {
 	if (/for (\S+=.*?\[.*?) in/) {
	    print "$1\n";
	} elsif (/FORM.*?`(.*?)'.*? for (.*?) in/) {
	    print "$1=$2\n";
	} else {
	    warn "nowt for $_";
	}
    }
}

1;
