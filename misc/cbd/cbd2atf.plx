#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

my $base = '';
my $entry = '';
my $lnum = 1;
my $pos = '';

GetOptions(

    'base:s'=>\$base,
    'pos:s'=>\$pos,
    
    );

$pos = 'PN' unless $pos;
$base = '{d}' unless $base;

$pos = undef if $pos eq '*';
$base = undef if $base eq '*';

my $using = 0;

while (<>) {
    if (/\@entry/ && (!$pos || /\s$pos\s*$/)) {
	$entry = $_;
	$using = 1;
    } elsif (/\@bases/ && $using) {
	# This is not good enough for all possible base selectors but
	# it is good enough to select by determinatives, for example
	if (!$base || /$base/) {
	    make_atf($entry,$_);
	}
	$using = 0;
    } elsif (/\@end\s+entry/) {
	$using = 0;
    }
}

sub make_atf {
    my($e,$b) = @_;
    chomp($e,$b);
    $e =~ s/\@entry\S*\s+(.*?)\s*$/$1/;
    $e =~ s/\s+\[/[/; $e =~ s/\]\s+/]/;
    $b =~ s/^\@bases\s+//;
    my @b = split(/;\s+/,$b);
    foreach my $b (@b) {
	$b =~ s/\s+\(.*$//;
	print "\@h1 $b=$e\n";
	print "\#new $e\n";
	print "$lnum. $b\n\n"; ++$lnum;
    }
}

1;
