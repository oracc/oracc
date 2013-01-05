#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $tlit = 0;
my $tlit_dollar = 0;
my $tlat_dollar = 0;
while (<>) {
    if (/^\&/) {
	add_spacers();
	$tlit_dollar = $tlat_dollar = 0;
	$tlit = 1;
    } elsif (/^\$/) {
	if ($tlit) {
	    ++$tlit_dollar;
	} else {
	    ++$tlat_dollar;
	}
    } elsif (/^\s*$/) {
    } elsif (/^\@/) {
	if (/^\@translation/) {
	    $tlit = 0;
	}
	unless ($tlit) {
	    $tlat_dollar = 0;
	}
    } else {
	if ($tlit) {
	    $tlit_dollar = 0;
	} else {
	    $tlat_dollar = 0;
	}
    }
    print;
}

add_spacers();

######################################################

sub
add_spacers {
    while ($tlit_dollar > $tlat_dollar) {
	print "\n\n\$ (SPACER)\n\n";
	--$tlit_dollar;
    }
}

1;
