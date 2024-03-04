#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use Getopt::Long;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $drop_thresh = 0;
my $drop_numerical = 1;
my $keep_non_unicode = 0;
my $only_numerical = 0;
my $sign_thresh = 0;

GetOptions (
    "all:i"=>\$keep_non_unicode,
    "drop:i"=>\$drop_thresh,
    "freq:i"=>\$sign_thresh,
    "num:i"=>\$drop_numerical,
    "onlynum:i"=>\$only_numerical,
    );

$drop_numerical = 0 if $only_numerical;

$drop_numerical && warn "rep-filter.plx: dropping numerical values/signs\n";
$only_numerical && warn "rep-filter.plx: keeping only numerical values/signs\n";
warn "rep-filter.plx: keeping signs with total values >= $sign_thresh times\n";
warn "rep-filter.plx: dropping values in result signs which occur < $drop_thresh times\n";

while (<>) {
    next unless /^x(?:e|12)/ || $keep_non_unicode;
    chomp;
    my($sign, $count, $freq, @vals) = split(/\t/, $_);
    my @newvals = ();

    foreach my $v (@vals) {
	my $ok = 1;
	if ($drop_numerical && $v =~ /^\d/) {
	    $ok = 0;
	} else {
	    if ($only_numerical) {
		if ($v =~ /^\d+\(/) {
		    my ($c) = ($v =~ m,\s(\d+)/,);
		    if ($c < $drop_thresh) {
			$ok = 0;
		    }
		} else {
		    $ok = 0;
		}
	    } else {
		my ($c) = ($v =~ m,(\d+)/,);
		if ($c < $drop_thresh) {
		    $ok = 0;
		}
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
	my @nocaps = grep(defined&&length, map { /[A-Z]/ ? undef : $_ } @newvals);
	if ($#nocaps >= 0) {
	    print "$sign\t$count\t$freq\t", join("\t", @nocaps), "\n";
	} elsif ($#newvals >= 0) {
	    print "$sign\t$count\t$freq\t", join("\t", @newvals), "\n";
	}
    }
}

1;
