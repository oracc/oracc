#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

# mk-atf-reptab.plx was modified to work with some other form generation
# scripts; this script actually does create a replacement table.

while (<>) {
    if (/no FORM `(.*?)'.*?match(?:es)? for ([^\[]+\[.*?\]\S*) in glossary \S+\s*(.*?)$/) {
	my $form = $1;
	my $cgp = $2;
	print "$form\t$cgp\t\n";
    } elsif (/no FORM/) {
	warn "$.: failed to match 'no FORM' line\n";
    }
}

1;
