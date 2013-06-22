#!/usr/bin/perl
use warnings; use strict;
my $webdir = shift @ARGV;

die "web-p3colours.plx: must give webdir on command line\n"
    unless $webdir;

my $oracc_colours = "$ENV{'ORACC'}/lib/data/p2colours.txt";


my %colours = ();
my %classes = ();
my %map_c = (
    href=>'a:link',
    hover=>'a[onclick]:hover a[href]:hover',
    visited=>'a:visited',
    'lemm-href'=>'a.cbd:link',
    'lemm-hover'=>'a.cbd[onclick]:hover a.cbd[href]:hover',
    'lemm-visited'=>'a.cbd:visited',
    );

my $p3colours_file = find_p3colours();

if (!$p3colours_file || !open(C,$p3colours_file)) {
    die "p2-colours.plx: can't find `p3colours.txt'\n";
}

while (<C>) {
    next if /^\s*$/ || /^\#/;
    if (/^colou?r-def-(\S+)\s+(\S+)\s*$/) {
	$colours{$1} = $2;
    } elsif (/^colou?r-bind-(\S+)\s+(.*)$/) {
	my($colour,$tags) = ($1,$2);
	my @tags = grep(defined&&length, split(/[ ,]+/, $tags));
	foreach my $t (@tags) {
	    my($base,$bgfg) = ($t =~ /^(.*?)-([^-]+?\s*)$/);
	    if ($base) {
		${$classes{$base}}{$bgfg} = $colour;
	    } else {
		${$classes{$t}}{'only'} = $colour;
	    }
	}
    }
}
close(C);

open(CSS,">$webdir/css/p3colours.css")
    || die "p2-colours.plx: can't write to `$webdir/css/p3colours.css'\n";
select CSS;
foreach my $c (sort keys %classes) {
    foreach my $mapped_c (split(/\s+/, $map_c{$c} || $c)) {
        my $dot = ($mapped_c =~ /:/ ? '' : '.');
	if (${$classes{$c}}{'only'}) {
	    if ($c eq 'border') {
		print ".border-top    { border-top: 1px solid $colours{${$classes{$c}}{'only'}}; }\n";
		print ".border-bottom { border-bottom: 1px solid $colours{${$classes{$c}}{'only'}}; }\n";
		print ".border-left   { border-left: 1px solid $colours{${$classes{$c}}{'only'}}; }\n";
		print ".border-right  { border-right: 1px solid $colours{${$classes{$c}}{'only'}}; }\n";
	    } elsif ($c eq 'bborder') {
		print ".bborder-top { border-top: 2px solid $colours{${$classes{$c}}{'only'}}; }\n";
		print ".bborder-bot { border-bottom: 2px solid $colours{${$classes{$c}}{'only'}}; }\n";
	    } else {
		print "$dot$mapped_c { color: $colours{${$classes{$c}}{'only'}}; }\n";
	    }
	} else {
	    my $indent = ' ';
	    print "$dot$mapped_c {";
	    if (${$classes{$c}}{'bg'}) {
		print " background-color: $colours{${$classes{$c}}{'bg'}};";
		$indent = "\n    ";
	    }
	    if (${$classes{$c}}{'fg'}) {
		print "${indent}color: $colours{${$classes{$c}}{'fg'}};";
	    }
	    print " }\n";
	}
    }
}
close(CSS);

open(CSX,">$webdir/p2.csx");
print CSX "<css>\n";
open(CSS,"$webdir/p2.css");
while (<CSS>) {
    print CSX;
}
print CSX "</css>\n";
close(CSX);

system 'chmod', 'o+r', "$webdir/css/p3colours.css";

############################################################

sub
find_p3colours {
    my $p = `oraccopt`;
    my $up = '';
    while (1) {
	my $try = "${up}00lib/p2colours.txt";
	if (-r $try) {
	    return $try;
	}
	$up = "$up../";
	last unless -r "${up}00lib";
    }
    (-r $oracc_colours) ? $oracc_colours : '';
}

1;
