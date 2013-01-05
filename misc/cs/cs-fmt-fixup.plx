#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my @f = (<>);
for (my $i = 0; $i <= $#f; ++$i) {
    $f[$i] =~ s/\t/        /g;
    if ($f[$i] =~ /\(LEMMA/) {
	chomp($f[$i-1]);
	my $pad = (100 - length($f[$i-1]));
	$f[$i-1] .= (' 'x$pad);
	$f[$i] =~ /(\(.*$)/;
	$f[$i-1] .= ' '.$1;
	$f[$i] = '#DELETE#';
	$f[$i+1] =~ /(\(.*$)/;
	$f[$i-1] .= ' '.$1;
	$f[$i+1] = '#DELETE#';
	$f[$i-1] .= "\n";
    }
}

print grep !/^#DELETE#/, @f;

1;
