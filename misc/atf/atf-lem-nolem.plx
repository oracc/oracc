#!/usr/bin/perl
use warnings; use strict; use utf8; use open ':utf8';
binmode STDIN,':utf8';
$/ = "\n&";
my $filename = shift @ARGV;
open(YES,">lem/$filename");
open(NO,">nolem/$filename");
#open(IN,$filename);
while (<>) {
    my($pq) = (/([PQX]\d\d\d\d\d\d)/);
    tr/\cM//d;
    s/﻿//g;
    s/^/&/ unless /^&/;
    s/&$/\n/s;
    if (/^\#lem/m) {
	print YES "\n", $_, "\n";
    } else {
	print NO "\n", $_, "\n";
    }
}
close(YES);
close(NO);
1;
