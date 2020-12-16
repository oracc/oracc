#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";
use Getopt::Long;

my $forms = '';
my $glos = '';
my %forms = ();

GetOptions(
    'f:s'=>\$forms,
    'g:s'=>\$glos,
    );

open(F,$forms) || die;
while (<F>) {
    chomp;
    warn "$forms:$.: bad forms line\n" unless /^\S+$/;
    ++$forms{$_};
}
close(F);

open(G,$glos) || die;
while (<G>) {
    if (/^\@form\S*\s+(\S+)/) {
	print if $forms{$1};
    } else {
	print;
    }
}
close(G);

1;
