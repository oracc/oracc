#!/usr/bin/perl
#
# Trivial wrapper for the ORACC::ATF::Unicode.pm module, q.v.
#
# Steve Tinney 04/03/01, for PSD and ORACCI projects.
#
# v1.0.  Placed in the Public Domain.
use warnings; use strict; use open ':utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Getopt::Long;
use lib "$ENV{'ORACC'}/lib";
use ORACC::ATF::Unicode;

while (<>) {
    if (/^\s*$/ || /^[\&\#\@\$]/) {
	print;
    } elsif (s/^(\S+?[:.]\s+)//) {
	my $lnum = $1;
	chomp;
	my $l = ORACC::ATF::Unicode::acc2num($_);
	print "$lnum$l\n";
    } elsif (s/^(==\S+)//) {
	my $lnum = $1;
	chomp;
	my $l = ORACC::ATF::Unicode::acc2num($_);
	print "$lnum$l\n";
    } else {
	warn "$.: unhandled line: $_";
	print;
    }
}

1;
