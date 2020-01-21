#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;

my $proper = 0; # 1 to process proper nouns

my %count = ();
my %insts = ();
my $ninst = 0;

my $line_one = <>;
if ($line_one =~ /^\@fields/) {
    while (<>) {
	next if /^\s*$/;
	s/\t(.*)$//;
	my $inst = $1; $ninst = ($inst =~ tr/ / /) + 1;
	chomp;
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
    print "$c\t$count{$c}\t@{$insts{$c}}\n";
}

####################################################################################

sub count1 {
    my $i = shift;
    my %s = parse_sig(shift);
    return unless $proper || $s{'pos'} !~ /^[A-Z]N$/;
    if ($s{'base'}) {
	my $cont = $s{'cont'} || 'o';
	my $key = "$s{'cf'}\[$s{'gw'}\]$s{'pos'}/$s{'base'}\+$cont";
	$count{$key} += $ninst;
	push @{$insts{$key}}, $i;
    }
}

1;
