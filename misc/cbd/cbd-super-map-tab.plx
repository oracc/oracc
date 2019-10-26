#!/usr/bin/perl
use warnings; use strict; use integer;

my $nozero = shift @ARGV || '';
die "$0: only allowed argument is '-z' to suppress entries which are all zero\n"
    unless !$nozero || $nozero eq '-z';

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

if ($nozero) {
    @maps = grep (${$stats{$_}}[0] && ${$stats{$_}}[1] && ${$stats{$_}}[2], @maps);
}

print "\n";
open(T,'>map-status.txt');
tab('Additional: ');
print "Entries\tSenses\tBases\tBmaps\tForms\n\n";
print T "Entries\tSenses\tBases\tBmaps\tForms\n\n";
foreach my $f (@maps) {
    tab($f);
    print join("\t", @{$stats{$f}}), "\n";
    print T join("\t", @{$stats{$f}}), "\n";
}
close(T);

sub stats {
    my $file = shift;
    my ($e,$s,$b,$m,$f) = (`grep -c 'add entry' $file`,
			   `grep -c 'add sense' $file`,
			   `grep -c 'add base' $file`,
			   `grep -c 'map base' $file`,
			   `grep -c 'add form' $file`,
	);
    chomp($e,$s,$b,$m,$f);
    [ $e, $s, $b, $m, $f ];
}

sub tab {
    my $f = shift @_;
    my $l = $len - length $f;
    my $t = $l / 8 + ($l % 8 ? 1 : 0);
    print $f;
    print "\t"x$t;
    print T $f;
    print T "\t"x$t;
}

1;
