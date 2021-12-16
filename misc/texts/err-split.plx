#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

#
# err-split.plx [DATA-FILE] [LOG-FILE]
#
# Read DATA-FILE and emit two outputs, one with good lines, the other with error
# lines.  Error lines are determined by DATA-FILE:LINE_NUMBER in LOG-FILE.
#
# Outputs:
#
# errsplit.ok
# errsplit.er 
# 

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

my $datafile = '';
my $logfile = '';

GetOptions(
    'data:s'=>\$datafile,
    'log:s'=>\$logfile,
    );

open(L,$logfile) || die;

open(D,$datafile) || die;

my %err = ();

while (<L>) {
    if (/^$datafile:(\d+):/) {
	++$err{$1};
    }
}
close(L);

open(OK, '>errsplit.ok') || die;
open(ER, '>errsplit.er') || die;

while (<D>) {
    if ($err{$.}) {
	print ER;
    } else {
	print OK;
    }
}
close(D);
close(OK);
close(ER);

1;
