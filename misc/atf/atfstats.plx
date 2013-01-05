#!/usr/bin/perl
use warnings; use strict; use open ':utf8';

my @files = ();
my %PQ = ();
my $curr = '';
my $lem = 0;
my $lines = 0;
my $words = 0;
my $graph = 0;

if ($#ARGV >= 0) {
    @files = @ARGV;
} else {
    @files = <00atf/*.atf>;
    push @files, <00atf/*.txt>;
}

foreach my $f (@files) {
    open(F,$f);
    while (<F>) {
	if (/^\&(P\S+)/) {
	    if ($curr) {
		$PQ{$curr} = [$lines,$words,$lem,$graph];
		$lines = $words = $lem = $graph = 0;
	    }
	    $curr = $1;
	} elsif (/^\#lem:/) {
	    $lem += tr/:;/:;/;
	} elsif (/^[\@\#\$\|\<\>]/ || /^\s*$/) {
	    next;
	} else {
	    s/(?:\{\{|\}\})/ /g;
	    ++$lines;
	    $words += tr/ / /;
	    $graph += tr/ -{/ -{/;
	}
    }
}

$PQ{$curr} = [$lines,$words,$lem,$graph] if $curr;
    
print "P\tLN\tWD\tLM\tGR\n";
foreach my $pq (sort keys %PQ) {
    my($l,$w,$m,$g) = @{$PQ{$pq}};
    print "$pq\t$l\t$m\t$w\t$g\n";
}

1;
