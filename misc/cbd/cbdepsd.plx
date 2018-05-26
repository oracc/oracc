#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Hash;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

my %args = pp_args('/Users/stinney/orc/epsd2/00src/sux.glo');
my $h = undef;
if (($h = pp_hash(\%args))) {
    my @c = pp_hash_cfgws($h);
    my %c = (); @c{@c} = ();
    while (<>) {
	chomp;
	print "$_\n" unless exists $c{$_};
    }
} else {
    pp_diagnostics();
}

1;
