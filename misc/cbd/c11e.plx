#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use Getopt::Long;
use ORACC::CBD::Util;
use ORACC::CBD::C11e;
use ORACC::CBD::Validate;

# Test harness for CBD canonicalize module

my %args = ();
GetOptions(
    \%args,
    qw/bare check dry edit filter lang:s project:s trace vfields:s/,
    ) || die "unknown arg";

$args{'project'} = 'test' unless $args{'project'};
$args{'cbdlang'} = 'sux' unless $args{'cbdlang'};
$args{'projdir'} = "$ENV{'ORACC_BUILDS'}/$args{'project'}";

$args{'cbd'} = shift @ARGV;
my @cbd = pp_load(\%args);
pp_validate(\%args, @cbd);
@cbd = c11e(\%args, @cbd);
pp_cbd(\%args, @cbd);

1;

