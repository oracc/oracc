#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
my($project,$html) = @ARGV;
$project = "\U$project";
open(P,$html);
while (<P>) {
    if (s,(<h1\s*class=\".*?p3h2(.*?)\">.*?)</h1>,$1 [via $project]</h1>,) {
	print;
	while (<P>) {
	    print;
	}
    } else {
	print;
    }
}
close(P);
close(STDOUT);
1;
