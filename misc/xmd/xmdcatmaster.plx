#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

my $project = '';
GetOptions(
    'project:s'=>\$project,
    );

$project = `oraccopt` unless $project;

my $p = $project;
my $cat_master = '';
while (1) {
    if (-r "$ENV{'ORACC_BUILDS'}/$p/00lib/config.xml") {
	$cat_master = `oraccopt $p catalog-master-project`;
    }
    last if $cat_master;
    last unless $p =~ s#/[^/]+##;
}
warn "$0: found cat-master $cat_master\n" if $cat_master;
print $cat_master;

1;
