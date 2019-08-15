#!/usr/bin/perl
#
use warnings; use strict; use open ':utf8'; use utf8;
use lib "$ENV{'ORACC'}/lib";
use ORACC::ATF::Unicode;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
while (<>) {
    if (s/^(\#lem:\s+)//) {
	print $1;
	chomp;
	my @bits = grep(defined, split(/;\s+/, $_));
	for (my $i = 0; $i <= $#bits; ++$i) {
	    if ($bits[$i] =~ /\[/) {
		my($cf,$rest) = ($bits[$i] =~ /^(.*?)(\[.*)$/);
		print ORACC::ATF::Unicode::gconv($cf), $rest;
	    } else {
		print $bits[$i];
	    }
	    print '; ' if $i < $#bits;
	}
	print "\n";
    } else {
	print;
    }
}

1;
