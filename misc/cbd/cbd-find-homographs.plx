#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

use Data::Dumper;

# Preload one or more 01bld/LANG/bases-sigs.tab files

use Getopt::Long;
my @pre = ();

# if 1, sort the base sig components so differently ordered translits
# with same signs are homographs
my $sort = 0; 
my $verbose = 0;

GetOptions(
    'preload:s' => \@pre,
    sort=>\$sort,
    verbose=>\$verbose,
    );

my %h = ();
my %t = ();
my %seen = ();

warn "$0: using sorted sigs\n" if $sort;

if ($#pre >= 0) {
    foreach my $p (@pre) {
	unless (-r $p) {
	    my($proj,$lang) = ($p =~ /^(.*?):(.*?)$/);
	    if ($proj) {
		$p = "$ENV{'ORACC_BUILDS'}/$proj/01bld/$lang/base-sigs.tab";
	    }
	}
	open(P,$p) || die "$0: can't preload $p\n";
	while (<P>) {
	    chomp;
	    my @f = split(/\t/,$_);
	    if ($sort) {
		my $s = join('.',sort split(/\./,$f[2])) if $sort;
		warn "$0: preload: using $s for $f[2]\n" if ($verbose && $s ne $f[2]);
		$f[2] = $s;
	    }
	    my $r = "$f[1]==$f[0]";
	    next if $seen{$f[0],$r}++;
	    # ++$h{$f[2]} if $t{$f[2]}; # don't report dups in preloads
	    push @{$t{$f[2]}}, $r;
	}
    }
}

#open(D,'>pre.dump') || die; print D Dumper \%t; close(D);

# clear %seen so that singleton entries in preload will match incoming dups

%seen = ();

# Read bases-sig.tab entries from STDIN and add them to the dups

while (<>) {
    chomp;
    my @f = split(/\t/,$_);
    my $r = "$f[1]==$f[0]";
    next if $seen{$f[0],$r}++;
    if ($sort) {
	my $s = join('.',sort split(/\./,$f[2])) if $sort;
	warn "$0: input: using $s for $f[2]\n" if ($verbose && $s ne $f[2]);
	$f[2] = $s;
    }
    ++$h{$f[2]} if $t{$f[2]};
    push @{$t{$f[2]}}, $r;
}

foreach (keys %h) {
    my $o = join("\t", @{$t{$_}});
    $o =~ /==(.*?)\t/;
    print "$1\t$_ => $o\n";
}

1;
