#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDERR, ':utf8'; binmode STDOUT, ':utf8';
use lib '@@ORACC@@/lib';
use Getopt::Long;
use Data::Dumper;

my $type = shift @ARGV;

open(W,">00web/$type-report.html"); select W;
print "<html><head><title>$type Report</title></head><body><h1>$type Report</h1>";
open(I,"00etc/$type-zero.log");
print "<h2>$type entries with no matches</h2><table>";
while (<I>) {
    chomp; s/^.*?\t//;
    print "<tr><td style=\"width:15%\">0</td><td style=\"width:70%\">$_</tr>\n";
}
print "</table>";
close(I);
print "<h2>$type entries which have matches</h2><table>";
open(I,"00etc/$type-hits.log");
while (<I>) {
    chomp;
    my($n,$c) = split(/\t/,$_);
    print "<tr><td style=\"width:15%\">$n</td><td style=\"width:70%\">$c</tr>\n";
}
print "</table>";
close(I);
print "</body></html>";
1;
