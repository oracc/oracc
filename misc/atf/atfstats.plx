#!/usr/bin/perl
use warnings; use strict; use open ':utf8';

my @files = ();
my %PQ = ();
my $curr = '';
my $lem = 0;
my $X = 0;
my $lines = 0;
my $words = 0;
my $graph = 0;

my $total_l = 0;
my $total_n = 0;
my $total_u = 0;
my $total_w = 0;
my $total_x = 0;

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
		$PQ{$curr} = [$lines,$words,$lem,$X,$graph];
		$lines = $words = $lem = $X = $graph = 0;
	    }
	    $curr = $1;
	} elsif (/^\#lem:/) {
	    $lem += tr/:;/:;/;
	    my $tmp = $_;
	    $X += ($tmp =~ s/ X\s*(;|$)//g);
	    $tmp = $_; $total_n += ($tmp =~ s/ n\s*(;|$)//g);
	    $tmp = $_; $total_u += ($tmp =~ s/ u\s*(;|$)//g);
	} elsif (/^[\@\#\$\|\<\>]/ || /^\s*$/) {
	    next;
	} else {
	    if (/^\S+?\.\s/) {
		s/(?:\{\{|\}\})/ /g;
		++$lines;
		$words += tr/ / /;
		$graph += tr/ -{/ -{/;
	    }
	}
    }
}

$PQ{$curr} = [$lines,$words,$lem,$graph] if $curr;
    
print "P\tLN\tWD\tLM\tX\tGR\n";
foreach my $pq (sort keys %PQ) {
    my($l,$w,$m,$x,$g) = @{$PQ{$pq}};
    $total_l += $m;
    $total_w += $w;
    $total_x += $x;
    print "$pq\t$l\t$m\t$x\t$w\t$g\n";
}

print "Total w = $total_w\n";
print "Total l = $total_l\n";
print "Total x = $total_x\n";
print "Total n = $total_n\n";
print "Total u = $total_u\n";


1;
