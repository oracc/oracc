#!/usr/bin/perl -C0
use warnings; use strict;

my $line_num = 0;
my $primes = '';
my $force_primes = 0;
while (<>) {
    if (/^\&/ || /^\s*$/) {
	$line_num = -1;
	$force_primes = 0;
	$primes = '';
    } elsif (/^\@(obverse|reverse|column|left|right|top|bottom|edge|face|seal)/) {
	$line_num = 1;
    } elsif (!/^[\#\$\@]/) {
	newnum() if $line_num > 0;
    } elsif (/^\$\s*beginning\s+broken/) {
	$primes = "'";
	$force_primes = 1;
    }
    print;
}

sub
newnum {
    if (m/^(\S+?)\.?/) {
	my $tmp = $1;
	if ($force_primes) {
	    $primes = "'";
	} elsif ($tmp =~ s/(\'+)$//) {
	    $primes = $1;
	} else {
	    $primes = '';
	}
	if ($tmp =~ /^\d+$/) {
	    s/^\d+\'*\.?/$line_num$primes./;
	    ++$line_num;
	}
    } elsif (/^\t/) {
	s/^/$line_num$primes./;
	++$line_num;
    }
}

1;
