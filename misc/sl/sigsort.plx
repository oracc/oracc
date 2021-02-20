#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;
use Getopt::Long;

my $append = 0;

GetOptions(
    append=>\$append,
    );

if ($append) {
    while (<>) {
	chomp;
	my($last) = /\t(\S+)$/;
	my $s = join('.', sort(split(/\./,$last)));
	print "$_\t$s\n";
    }
} else {
    print join('.', sort(split(/\./,$ARGV[0])));
}

1;
