#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

my $l = 0;
my $no = 0; # no lem provided, i.e., text unlemmatized
my $nu = 0; # lem as n(umber)
my $u = 0;
my $w = 0;
my $x = 0;

my @files = ();

if ($#ARGV >= 0) {
    @files = @ARGV;
} else {
    @files = <00atf/*.atf>;
    push @files, <00atf/*.txt>;
}

open(OX9,"ox -9 @files |") || die "$0: unable to open ox -9 @files for input\n";
while (<OX9>) {
    ++$w;
    my($t) = (/=(\S+)/);
    if ($t) {
	if ($t eq 'u') { ++$u }
	elsif ($t eq 'n' || $t eq 'n/a') { ++$nu }
	elsif ($t eq 'X') { ++$x }
	elsif ($t =~ /\[/ || $t =~ /^[A-Z]\S+$/) { ++$l }
	elsif ($t =~ 'L') {
	    # ignore 'wrong language' lem L for now
	} else {
	    warn $_;
	}
    } else {
	++$no;
    }
}
close(OX9);

print "Total word = $w\n";
print "Total lem = $l\n";
print "Total x-lem = $x\n";
print "Total n-lem = $nu\n";
print "Total u-lem = $u\n";
print "Total not lem = $no\n";

1;
