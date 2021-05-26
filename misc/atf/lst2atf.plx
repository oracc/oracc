#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

my $project = '';
my $x = '0001';
GetOptions(
    'project:s'=>\$project,
    );

die "$0: must give project on command line\n" unless $project;

print <<EOF;
\&X121212=X
#project: $project
#atf: use unicode
#atf: use math
#atf: use legacy
EOF

while (<>) {
    print "$x. $_";
    ++$x;
}

1;
