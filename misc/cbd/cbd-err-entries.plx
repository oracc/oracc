#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my $g = shift @ARGV;

die "$0: no glossary $g\n" unless -r $g;

my @err = `grep -Hn \@entry $g`;
my %err = ();
foreach my $e (@err) {
    my($err,$ent) = ($e =~ /^(.*?:.*?:)(.*?)\s*$/);
    $ent =~ s/^.?\@entry\S*\s+//;
    $err{$ent} = $err;
}

# print Dumper \%err; exit 0;

while (<>) {
    /^(.*?)\t/;
    if ($err{$1}) {
	print "$err{$1}\t$_";
    } else {
	warn "no errline for $1\n";
	print;
    }
}

1;
