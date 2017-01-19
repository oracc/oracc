#!/usr/bin/perl
use warnings; use strict;

my @opts = `xsltproc -xinclude /usr/local/src/oracc/misc/admin/config-options.xsl /home/oracc/agg/projects/all-projects.xml`;
my %opts = ();
foreach (@opts) {
    s/\t.*$//; chomp;
    ++$opts{$_};
}

my @rng = `xsltproc /usr/local/src/oracc/misc/admin/config-rng-doco.xsl /usr/local/src/oracc/doc/ns/xpd/1.0/xpd-source.rng`;
chomp @rng;
my %rng = (); @rng{@rng} = ();

my @bad = ();

foreach (keys %opts) {
    unless (exists $rng{$_}) {
	push (@bad, $_) unless /^\%/;
    }
}

print join("\n", sort @bad), "\n";

1;
