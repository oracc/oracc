#!/usr/bin/perl
use warnings; use strict; use utf8; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use Getopt::Long;
use Pod::Usage;

my $file = '';
my $chunk = 0;
my $lines = 1000000;
my $count = 0;
my $printing = 0;

$file = sprintf("00atf/ur3_%d.atf", $chunk);
warn "file = $file\n";
open(A,">$file");
while (<>) {
    if (/^\&/ && $count >= $lines) {
	close(A);
	++$chunk;
	$file = sprintf("00atf/ur3_%d.atf", $chunk);
	warn "file = $file\n";
	$count = 0;
	open(A,">$file");
    }
    print A;
    ++$count;
}

close(A);
1;
