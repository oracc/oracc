#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;

my $proper = 0; # 1 to process proper nouns

my %count = ();

my $line_one = <>;
if ($line_one =~ /^\@fields/) {
    while (<>) {
	s/\t.*$//;
	chomp;
	if (/^\{/) {
	    s/^.*?:://;
	    foreach my $s (split(/\+\+/, $_)) {
		count1($s);
	    }
	} else {
	    count1($_);
	}
    }
} else {
    die "$0: I only work with .sig files that start with a '\@fields' line. Stop.\n";
}

foreach my $c (sort keys %count) {
    print "$c\t$count{$c}\n";
}

####################################################################################

sub count1 {
    my %s = parse_sig(shift);
    return unless $proper || $s{'pos'} !~ /^[A-Z]N$/;
    if ($s{'base'}) {
	++$count{"$s{'cf'}\[$s{'gw'}\]$s{'pos'}/$s{'base'}"};
    }
}

1;
