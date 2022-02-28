#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $verbose = 0;

while (<>) {
    my $query = $_; chomp $query;
    $query =~ s/^!//;
    $query =~ s/\t.*//;
    $query =~ s/N-T\s+/N-T/;
    if ($query =~ /N-T(\d+)/) {
	my $z = '0'x(4-length($1));
	$query =~ s/T/T$z/;
    }
    $query =~ tr/-,.:;()//d;
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
