#!/usr/bin/perl
use warnings; use strict;

use Data::Dumper;

my %app = ();
my %class = ();
my %classmap = (
    medium=>'mid',
    outline=>'border',
    alternate=>'heading-bg',
    highlight=>'hover-fg',
    print=>'#000',
    text=>'#333',
    page=>'#fff',
    );
my @colours = qw/dark medium light outline alternate text page highlight/;
my %usedfor = ();

my $p3colours = find_p3colours() || die "esp2-p3colours.plx: can't find p3colours.txt\n";
open(P, $p3colours) || die "esp2-p3colours.plx: can't open $p3colours\n";
while (<P>) {
    if (/^colou?r-def-(.*?)\s+(\S+)$/) {
	$class{$1} = $2;
    } elsif (/^colou?r-bind-(.*?)\s/) {
	my $class = $1;
	my $usedfor = '';
	if (/\s+(.+)$/) {
	    my $ulist = $1;
	    foreach my $u (split(/\s+/, $ulist)) {
		$usedfor{$u} = $class;
	    }
	}
    }
}
close(P);

#print Dumper \%class;
#print Dumper \%usedfor;

foreach my $c (@colours) {
    if ($class{$c}) {
	$app{$c} = $class{$c};
    } elsif ($classmap{$c}) {
	my $m = $classmap{$c};
	if ($m =~ /^#/) {
	    $app{$c} = $classmap{$c};
	} elsif ($class{$m}) {
	    $app{$c} = $class{$m};
	} elsif ($usedfor{$m} && $class{$usedfor{$m}}) {
	    $app{$c} = $class{$usedfor{$m}};
	} else {
	    warn "esp2-p3colours.plx: app:colours value for $c: no map for $m in $p3colours\n";
	}
    } else {
	warn "esp2-p3colours.plx: no definition for app:colours $c\n";
    }
}



sub
find_p3colours {
    my $p = `oraccopt`;
    my $up = '';
    while (1) {
	my $try = "${up}00lib/p3colours.txt";
	if (-r $try) {
	    return $try;
	}
	$up = "$up../";
	last unless -r "${up}00lib";
    }
    undef;
}

