#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
my @err = (<>);
for (my $i = 0; $i <= $#err; ++$i) {
    if ($err[$i] =~ /^Error/) {
	if ($err[$i+1] =~ /does not satisfy the "NMTOKEN" type/) {
	    $i+=2;
	} else {
	    print $err[$i];
	    print $err[++$i];
	}
    } else {
	print $err[$i];
    }
}
1;
