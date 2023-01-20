#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

my $not = 0;

GetOptions(
    );

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::SL::BaseC;

ORACC::SL::BaseC::init();
ORACC::SL::BaseC::pedantic();

# feed this a list of qualified values, like a(A)
while (<>) {
      chomp;
      my $p = ORACC::SL::BaseC::is_sign("$_;p");
      if ($p) {
	  my($v,$q) = (/^(.*?)\((.*?)\)$/);
	  my $qid = ORACC::SL::BaseC::is_sign($q);
	  if ($p ne $qid) {
	      my $forms = ORACC::SL::BaseC::is_sign("$p;forms");
	      my ($var) = ($forms =~ m#$qid/(~\S+)#);
	      my $psn = ORACC::SL::BaseC::sign_of($p);
	      if ($var) {
		  my $cq = ORACC::SL::BaseC::sign_of($qid);
		  print "$psn\t$var\t$cq\t$v\n";
	      } else {
		  warn "$0: needed $qid from $p;forms => $forms but it's not there\n";
	      }
	  } else {
	      my $cq = ORACC::SL::BaseC::sign_of($p);
	      print "$cq\t$v\n";
	  }
      }
}

1;
