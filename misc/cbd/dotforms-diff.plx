#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;



GetOptions(
    );


# dotforms-diff.plx FORMS1 FORMS2
#
# print forms from FORMS2 that aren't in FORMS1
my %f1 = ();
my($f1,$f2) = @ARGV;

die "$0: must give FORMS1 and FORMS2 on command line\n" unless $f1 && $f2;
die "$0: can't open FORMS1 $f1\n" unless open(F1,$f1);
die "$0: can't open FORMS2 $f2\n" unless open(F2,$f2);

while (<F1>) {
    ++$f1{dotforms_c18e($f1, $_)};
}
close(F1);

while (<F2>) {
    my $f2 = dotforms_c18e($f2, $_);
    print "$f2\n" unless $f1{$f2};
}
close(F2);

sub dotforms_c18e {
    my ($f,$tmp) = @_;
    my($cgp,$form) = split(/\t/, $tmp);
    if ($cgp) {
	$cgp =~ s/^\s*//; $cgp =~ s/\s*$//;
	$form =~ s/^\s*//; $form =~ s/\s*$//;
	$form =~ s/\s+/ /;
	return "$cgp\t$form";
    } else {
	warn "$f:$.: no <TAB> in forms line\n";
	return $tmp;
    }
}

1;
