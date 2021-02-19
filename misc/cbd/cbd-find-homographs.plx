#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

use Data::Dumper;

# Preload one or more 01bld/LANG/bases-sigs.tab files

use Getopt::Long;
my @pre = ();
GetOptions(
    'preload:s' => \@pre
    );

my %h = ();
my %t = ();
my %seen = ();

if ($#pre >= 0) {
    foreach my $p (@pre) {
	open(P,$p) || die "$0: can't preload $p\n";
	while (<P>) {
	    chomp;
	    my @f = split(/\t/,$_);
	    my $r = "$f[1]==$f[0]";
	    next if $seen{$f[0],$r}++;
	    # ++$h{$f[2]} if $t{$f[2]}; # don't report dups in preloads
	    push @{$t{$f[2]}}, $r;
	}
    }
}

open(D,'>pre.dump') || die; print D Dumper \%t; close(D);

# clear %seen so that singleton entries in preload will match incoming dups

%seen = ();

# Read bases-sig.tab entries from STDIN and add them to the dups

while (<>) {
    chomp;
    my @f = split(/\t/,$_);
    my $r = "$f[1]==$f[0]";
    next if $seen{$f[0],$r}++;
    ++$h{$f[2]} if $t{$f[2]};
    push @{$t{$f[2]}}, $r;
}

foreach (keys %h) {
    print "$_ => @{$t{$_}}\n";
}

1;
