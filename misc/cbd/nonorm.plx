#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

while (<>) {
    unless (/\$\(/) {
	if (/\%([-a-z]+)/) {
	    my $lang = $1;
	    if ($lang =~ /^sux/) {
		1 while s/\s+\$\S+//;
	    }
	} else {
	    1 while s/\s+\$\S+//;
	}
    }
    print;
}

1;
