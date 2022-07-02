#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use ORACC::XML;
use ORACC::NS;

use Getopt::Long;

my $lang = '';

GetOptions(
    'lang:s'=>\$lang,
    );

# load articles.xml
my $glo = "01bld/$lang/articles.xml";
if (-r $glo) {
    my $x = load_xml($glo);
    my @e = tags($x, $CBD, 'entry');
    foreach my $e (@e) {
	my $n = $e->getAttribute('n');
	my $x = xid($e);
	print "$n\t$x\n";
    }
}

1;
