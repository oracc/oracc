#!/usr/bin/perl
use warnings; use strict; use integer;

my $len = 0;
my %stats = ();

my @maps = <01map/*.map>;

my @noreport = (
    '01map/epsd2-emesal~sux-x-emesal.map',
    '01map/epsd2-names~qpn.map',
    );
my %noreport = (); @noreport{@noreport} = ();

@maps = grep(!exists($noreport{$_}), @maps);

my @lmaps = sort { length($b) <=> length($a) } @maps;

$len = length($lmaps[0]); $len = (($len / 8) + ($len % 8 ? 1 : 0)) * 8;

foreach my $f (@maps) {
    $stats{$f} = stats($f);
}

@maps = sort { ${$stats{$b}}[0] <=> ${$stats{$a}}[0] } @maps;

tab('');
print "Entries\tSenses\tBases\n";
foreach my $f (@maps) {
    tab($f);
    print join("\t", @{$stats{$f}}), "\n";
}

sub stats {
    my $f = shift;
    my ($e,$s,$b) = (`grep -c 'add entry' $f`,
		     `grep -c 'add sense' $f`,
		     `grep -c 'add base' $f`,
	);
    chomp($e,$s,$b);
    [ $e, $s, $b ];
}

sub tab {
    my $f = shift @_;
    my $l = $len - length $f;
    my $t = $l / 8 + ($l % 8 ? 1 : 0);
    print $f;
    print "\t"x$t;
}

1;
