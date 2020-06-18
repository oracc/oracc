#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

my %b = ();
my %e = ();
my $bad = 0;
my $glo = shift @ARGV;
die "$0: must give glossary on commandline\n" unless $glo;
die "$0: glossary $glo not readable\n" unless -r $glo;

my @map = (<>);
foreach my $m (@map) {
    my $errline = '';
    $m =~ s/^(.*?:.*?:\s*)// && ($errline = $1);
    if ($m =~ /^new bases (.*?) => (.*)$/) {
	my($cgp,$bases) = ($1,$2);
	$cgp =~ s/(\[.*?\])/ $1 /;
	if ($b{$cgp}) {
	    ++$bad;
	} else {
	    $b{$cgp} = $bases;
	}
	push @{$e{$cgp}},$errline;
    }
}

if ($bad) {
    foreach my $c (sort keys %e) {
	next unless $#{$e{$c}} > 0;
	foreach my $e (@{$e{$c}}) {	    
	    $c =~ s/\s+(\[.*?\])\s+/$1/;
	    warn "$e $c: repeated new bases\n";
	}
    }
    exit 1;
}

my $next_bases = '';
open(G,$glo) || die "$0: couldn't open $glo\n";
while (<G>) {
    if (/^[-+]?\@entry\S*\s+(.*?)\s*$/) {
	$next_bases = $b{$1};
	print;
    } elsif (/^\@bases/) {
	if ($next_bases) {
	    print $next_bases, "\n";
	} else {
	    print;
	}
    } else {
	print;
    }
}

1;
