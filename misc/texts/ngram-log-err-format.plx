#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDERR, ':utf8'; binmode STDOUT, ':utf8';
use lib '@@ORACC@@/lib';
use Getopt::Long;
use Data::Dumper;

my $where = shift @ARGV;
my $what = shift @ARGV;

my %glo = ();
open(GLO,"$where/$what-glo.log");
while (<GLO>) {
    chomp;
    />>\s+(.*?)\s*$/;
    my $x = $1; $x =~ s/\s+=>.*$//;
    $glo{$x} = $_;
}
close(GLO);

open(N,'>ngram.dump'); print N Dumper \%glo; close(N);

open(LOG,"$where/$what-zero.log");
while (<LOG>) {
    chomp;
    /\t(.*?)\s*$/;
    if ($glo{$1}) {
	print $glo{$1}, "\n";
    } else {
	warn "$0: $where/$what-glo.log doesn't have: $1\n";
    }
}
close(LOG);

1;
