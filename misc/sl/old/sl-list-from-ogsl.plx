#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

my $asl = '';
my $stdin = 0;

GetOptions(
    s=>\$stdin,
    );

# add @list entries from ogsl--only if they aren't already in the
# .asl named on the command line

unless ($stdin) {
    $asl = shift @ARGV;
    
    die "$0: no asl on command line. Stop.\n" unless $asl;
    die "$0: unreadable asl $asl. Stop.\n" unless -r $asl;
}

my $ogsl = "$ENV{'ORACC_BUILDS'}/ogsl/00lib/ogsl.asl";
die "$0: can't find $ogsl. Stop.\n" unless -r $ogsl;

my %lists = ();
my %asl_lists = ();

my $curr = '';

open(O, $ogsl) || die "$0: failed to open $ogsl. Stop\n";
while (<O>) {
    if (/^\@sign\s+(.*?)\s*$/) {
	$curr = $1;
    } elsif (/^\@form\s+\S+\s+(.*?)\s*$/) {
	$curr = $1;
    } elsif (/^\@list\s+(.*?)\s*$/) {
	push @{$lists{$curr}}, $1;
    }
}
close(O);

my @asl_in = ();
if ($stdin) {
    @asl_in = (<>);
} else {
    open(A,$asl) || die "$0: failed to open $asl. Stop\n";
    @asl_in = (<A>);
    close(A);
}

my @asl = ();
foreach (@asl_in) {
    if (/^\@sign\s+(.*?)\s*$/) {
	$curr = $1;
	push @asl, $_;
    } elsif (/^\@form\s+\S+\s+(.*?)\s*$/) {
	$curr = $1;
	push @asl, $_;
    } elsif (/^\@list\s+(.*?)\s*$/) {
	push @{$asl_lists{$curr}}, $1;
    } else {
	push @asl, $_;
    }
}

foreach my $a (@asl) {
    print $a;
    if ($a =~ /^\@sign\s+(.*?)\s*$/ || $a =~ /^\@form\s+\S+\s+(.*?)\s*$/) {
	my $sf = $1;
	my %l = ();
	if ($asl_lists{$sf}) {
	    @l{@{$asl_lists{$sf}}} = ();
	}
	if ($lists{$sf}) {
	    @l{@{$lists{$sf}}} = ();
	}
	print map { "\@list $_\n" } sort keys %l;
    }
}

sub do_list {
}

1;
