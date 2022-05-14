#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

while (<>) {
    if (/no FORM `(.*?)'.*?match(?:es)? for ([^\[]+\[.*?\]\S*) in glossary \S+\s*(.*?)$/) {
	my $form = $1;
	my $cgp = $3 || $2;
	$cgp =~ s/\]$/\]XX/;
	$cgp =~ s/\[(.*?)\]/ [$1] /;
	print "$cgp\t\@form $form /$form #~\n";
    } elsif (/no FORM/) {
	warn "$.: failed to match 'no FORM' line\n";
    }
}

1;
