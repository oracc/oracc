#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
#######
while (<>) {
    if (/<author|<editor/ && !/,/) {
	s#<(author|editor)>(.*?)</(?:auth|edit)or>#revname($1,$2)#e;
    }
    print
}
#######
sub
revname {
    my($tag,$name) = @_;
    $name =~ s/^\s+//; $name =~ s/\s+$//;
    $name =~ s/^(.*?)\s+(\S+)$/$2, $1/
	unless $name =~ /^et\.?\s+al\.?$/;
    my $newname = "<$tag>$name</$tag>";
    my ($last,$first) = ($newname =~ /^(.*?),\s+(.*?)$/);
    if ($first) {
	$first =~ s/\.\s+/\./g;
	$newname = "$last, $first";
    }
    $newname;
}
1;
