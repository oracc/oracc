#!/usr/bin/perl
use warnings; use strict; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $length = '';

while (<>) {
    if (/^\@entry(\*?)\s+(\S+)(.*?)$/) {
	my($star,$nn,$rest) = ($1,$2);
	chomp($rest);
	$star = '' unless $star;
	my $fixed_nn = fix_nn($nn);
	print "\@entry$star\t$fixed_nn$rest\n";
	print "\@length\t$length\n" if $length;
    } elsif (/\@form/) {
	s/\$$nn/\$fixed_nn/;
    } else {
	print;
    }
}

sub
fix_nn {
    my $nn = shift;
    $nn =~ s/([aeiu])-(\1)/$1'$1/g;
    $nn =~ s/-(\p{Lu})(\p{Ll}|-)/-\l$1$2/g;
    $nn =~ tr/-.//d;
    $length = $nn;
    $nn =~ tr/āēīūâêîûĀĒĪŪÂÊÎÛ/aeiuaeiuAEIUAEIU/;
    $length = '' unless $nn ne $length;
    $nn;
}

1;
