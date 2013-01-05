#!/usr/bin/perl
use warnings; use strict;
my $dir;
my $ext = shift @ARGV || 'xtf';
while (<>) {
    if (/:$/) {
	chomp;
	s/:$//;
	$dir = $_;
    } elsif (/\.$ext$/o) {
	print "$dir/$_";
    }
}
1;
