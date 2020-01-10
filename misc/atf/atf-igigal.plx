#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $i = 0;

while (<>) {
#    print STDERR "$i " unless $i++ % 1000;
    if (/\sigi.*?[gŋ]al₂/) {
	s/\.\.\.\]-([gŋ]al₂)/...] $1/g;
	igigal();
    } else {
	print;
    }
}

sub igigal {
    chomp;
    my $done = '';
#    warn "$_\n" if $i > 822000;
    while (1) {
	my($pre,$ing,$pst) = /^(.*?\s)(igi.*?[gŋ]al₂)(.*?)$/;
	$done .= $pre;
	if (igigal_ok($ing)) {
	    $ing =~ tr/-/ /;
	}
	$done .= $ing;
	unless ($pst =~ /\sigi.*?[gŋ]al₂/) {
	    $done .= $pst;
	    last;
	} else {
	    $_ = $pst;
	}
    }
    print $done, "\n";
}

sub igigal_ok {
    my @ing = split(/[-\s]/, $_[0]);
    my $ok = 1;
    if ($#ing >= 2) {
	foreach (@ing) {
	    tr/[]#*!?<>//d;
	    $ok = 0
		unless /^[0-9n]+\(/ || /^(?:\.\.\.|n|x|igi|[gŋ]al₂|la₂)$/;
	}
	warn "$_[0] not ok\n" unless $ok;
	$ok;
    } else { # silently skip processing of igi-gal₂
	return 0;
    }
}

1;
