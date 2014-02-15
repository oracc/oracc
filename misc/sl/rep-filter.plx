#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use Getopt::Long;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $drop_thresh = 0;
my $drop_numerical = 1;
my $sign_thresh = 0;

GetOptions (
    "drop:i"=>\$drop_thresh,
    nonum=>\$drop_numerical,
    "freq:i"=>\$sign_thresh,
    );

$drop_numerical && warn "rep-filter.plx: dropping numerical values/signs\n";
warn "rep-filter.plx: keeping signs with total values >= $sign_thresh times\n";
warn "rep-filter.plx: dropping values in result signs which occur < $drop_thresh times\n";

while (<>) {
    next unless /^x12/;
    chomp;
    my($sign, $count, $freq, @vals) = split(/\t/, $_);
    my @newvals = ();

    foreach my $v (@vals) {
	my $ok = 1;
	if ($drop_numerical && $v =~ /^\d/) {
	    $ok = 0;
	} else {
	    my ($c) = ($v =~ m,(\d+)/,);
	    if ($c < $drop_thresh) {
		$ok = 0;
	    }
	}
	push(@newvals, $v) if $ok;
    }

    my $vtotal = 0;
    foreach my $v (@vals) {
	my ($c) = ($v =~ m,(\d+)/,);
	$vtotal += $c;
    }

    if ($vtotal >= $sign_thresh) {
	my @nocaps = grep(defined&&length, map { /[A-Z]/ || $_ } @newvals);
	if ($#nocaps >= 0) {
	    print "$sign\t$count\t$freq\t", join("\t", @nocaps), "\n";
	} elsif ($#newvals >= 0) {
	    print "$sign\t$count\t$freq\t", join("\t", @newvals), "\n";
	}
    }
}

1;
