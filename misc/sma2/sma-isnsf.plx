#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::SMA2::NSF;

use Getopt::Long;

my @input = ();
my $orth = '';
my $root = '';
my $table = '';

GetOptions(
    'orth:s'=>\$orth,
    'root:s'=>\$root,
    'table:s'=>\$table,
    );

if ($table) {
    @input = `cat $table`; chomp @input;
} else {
    @input = (<>); chomp @input;
}

foreach my $l (@input) {
    my $tlit = '';
    if ($l =~ /\t/) {
	($root,$orth,$tlit) = split(/\t/,$l);
    } else {
	$tlit = $l;
    }
    my $n = is_nsf($root,$orth,split(/-/,$tlit));
    my $h = get_last_nsf();
#    warn Dumper($h), "\n";
    my @m = @{$$h{'nsf'}}[1..$#{$$h{'nsf'}}];
    print "$l\t",join('.', grep(length,@m)),"\n" if $n;
}

1;
