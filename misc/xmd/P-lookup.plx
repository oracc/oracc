#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $verbose = 0;

while (<>) {
    my $query = $_;
    $query =~ tr/-,.:;()//d;
    $query =~ s/\t.*//;
    $query =~ s/^\s*(.*?)\s*$/$1/;
    $query =~ s/\s*[=\+].*$//;
    $query =~ s/\s+/_/g;
    if ($query =~ /[a-z0-9]/i) {
	warn "$query\n" if $verbose;
	my @res = `se \#cdli \!cat $query`;
	if ($#res >= 0) {
	    chomp @res;
	    @res = map { s/^cdli://; $_ } @res;
	    print "@res\t$_";
	    next;
	}
    }
    print;
}

1;
