#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
use Getopt::Long;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
my $xml = 0;
GetOptions(
    'x'=>\$xml,
    );
my $keyid = 'aaaaaa';
my @lines = (<>); chomp(@lines);
my %key2line = ();
my %line2key = ();
foreach my $l (@lines) { 
    $key2line{$keyid} = $l; 
    $line2key{$l} = $keyid;
    ++$keyid;
}
open(T,">$$.gdlsort");
print T map { make_key($_) } @lines;
close(T);
my @sorted = `psdsort -G $$.gdlsort`; chomp(@sorted);
if ($xml) {
    my $code = 0;
    print '<?xml version="1.0" encoding="utf-8"?>',"\n";
    print '<gdlsort>';
    foreach my $k (@sorted) {
	$k =~ /(\S+)$/;
	print "<sort key=\"$key2line{$1}\" code=\"$code\"/>";
	++$code;
    }
    print '</gdlsort>';
} else {
    foreach my $k (@sorted) { 
	$k =~ /(\S+)$/ && print $key2line{$1}, "\n"; 
    }
}
#######################################################
sub
make_key {
    my $l1 = shift;
    my $l2 = $l1;
    my $k = $line2key{$l1};
    my @bits = ();
    $l1 =~ s/\{.*?\}//g;
    "$l1 $l2 $k\n";
}
1;
