#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my $obin = "$ENV{'ORACC_BUILDS'}/bin";
my $b = "01tmp/xis/";

while (<>) {
    next unless /^o\d/;
    chomp;
    my($o,$f,$i) = split(/\t/, $_);
    my $d = "$b/$o";
    system 'mkdir', '-p', $d;
    my $inl = $i; $inl =~ tr/ /\n/;
    open(I,">$d/i.lst"); print I $inl; close I;
    my @x = `$obin/pg2 -s period -t -n 0 -P 1000000 <$d/i.lst`;
    warn "$o $f\n";
    warn @x;
    my @stat = (0,0,0);
    foreach (@x) {
	my ($p,$i) = split(/\t/, $_); $i =~ s/\s*$//;
	if ($p =~ /^Uruk V/) {
	    $stat[0] = $i =~ tr/ /\n/ +1;
	    open(O,">$d/${o}_0.tis"); print O $i, "\n"; close O;
	} elsif ($p =~ /^Uruk IV/) {
	    $stat[1] = $i =~ tr/ /\n/ +1;
	    open(O,">$d/${o}_1.tis"); print O $i, "\n"; close O;
	} elsif ($p =~ /^Uruk III/) {
	    $stat[2] = $i =~ tr/ /\n/ +1;
	    open(O,">$d/${o}_2.tis"); print O $i, "\n"; close O;
	}
    }
    open(D,">$d/data");
    print D "oid\t$o\ntotal\t$f\nV\t$stat[0]\nIV\t$stat[1]\nIII\t$stat[2]\n";
    close D;
}

1;
