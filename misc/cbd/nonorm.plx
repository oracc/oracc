#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

while (<>) {
    unless (/\$\(/) {
	if (/\%(\S+)/) {
	    my $lang = $1;
	    if ($lang =~ /^sux/) {
		s/\s+\$\S+//;
	    }
	} else {
	    s/\s+\$\S+//;
	}
    }
    print;
}

1;
