#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;

binmode STDIN, ':utf8';
binmode STDOUT, ':utf8';
binmode STDERR, ':utf8';

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::Log::Util;

# autofix missing #why: entries in CBD--not good enough for ePSD
# but OK for epsd2/admin/names

my $verbose = 0;

while (<>) {
      if (/must give single line \#why:/) {
	  my($f,$l) = (/^(.*?):(.*?):/);
	  my $line = log_get_line($f,$l);
	  $line .= "#why: because\n";
	  log_set_line($l,$line);
      }
}

log_term();

1;
