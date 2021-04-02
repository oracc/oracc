#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;

use Getopt::Long;
my $entry = 0; # 1 to print entry freqs not base freqs
my $instprint = 0; # 1 to print instances
my $proper = 0; # 1 to process proper nouns
GetOptions(
    entry=>\$entry,
    instprint=>\$instprint,
    proper=>\$proper,
    );

my %count = ();
my %insts = ();
my $ninst = 0;

my $line_one = '';
do {
    $line_one = <>;
    last unless defined $line_one;
} until ($line_one =~ /^\@fields/);

if ($line_one =~ s/^\@fields\s+//) {
    chomp($line_one);
    my @f = split(/\s+/,$line_one);
    my $freq_index = 0;
    my $inst_index = 0;
    for (my $i = 0; $i <= $#f; ++$i) {
	if ($f[$i] eq 'freq') {
	    $freq_index = $i;
	} elsif ($f[$i] eq 'inst') {
	    $inst_index = $i;
	}
    }
    while (<>) {
	next if /^\s*$/;
	chomp;
	my @f = split(/\t/,$_);
	my $inst = '';
	if ($freq_index) {
	    $ninst = $f[$freq_index];
	}
	if ($inst_index) {
	    $inst = $f[$inst_index];
	    $ninst = (($inst =~ tr/ / /) + 1)
		unless $freq_index;
	}
	if (/^\{/) {
	    s/^.*?:://;
	    foreach my $s (split(/\+\+/, $_)) {
		count1($inst,$s);
	    }
	} else {
	    count1($inst,$_);
	}
    }
} else {
    die "$0: I only work with .sig files that start with a '\@fields' line. Stop.\n";
}

foreach my $c (sort keys %count) {
    print "$c\t$count{$c}\t";
    print "@{$insts{$c}}" if $instprint;
    print "\n";
}

####################################################################################

sub count1 {
    my $i = shift;
    my %s = parse_sig(shift);
    return unless $proper || $s{'pos'} !~ /^[A-Z]N$/;
    if ($s{'base'}) {
	my $cont = $s{'cont'} || 'o';
	my $key = "$s{'cf'}\[$s{'gw'}\]$s{'pos'}";
	$key .= "/$s{'base'}\+$cont" unless $entry;
	$count{$key} += $ninst;
	push @{$insts{$key}}, $i;
    }
}

1;
