#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC'}/lib";
use ORACC::CBD::Util;
use ORACC::CBD::Bases;
my $acd_rx = $ORACC::CBD::acd_rx;

my $curr_pos = '';
my %forms = ();

my @default_post = qw/a me še₃ ta/;
my @DN_PN_post = qw/a e me ra še₃ ta/;
my %morph = (); @morph{@DN_PN_post} = qw/a e meš ra eše ta/;

while (<>) {
    if (/^$acd_rx\@entry/) {
	/(\S+)\s*$/;
	$curr_pos = $1;
    } elsif (/^\@bases/) {
	print;
	my @b = bases_primaries($_); chomp @b;
	foreach my $b (@b) {
	    my $f = "$b /$b #~";
	    $forms{$b} = $f;
	    my @post = ($curr_pos eq 'DN' || $curr_pos eq 'PN') ? @DN_PN_post : @default_post;
	    foreach my $post (@post) {
		my $orth = "$b-$post";
		$f = "$orth /$b #~,$morph{$post}";
		$forms{$orth} = $f;
	    }
	}
    } elsif (s/^\@form\s*//) {
	chomp;
	s/\s+/ /;
	s/\s*$//;
	/^(\S+)/;
	my $orth = $1;
	$forms{$orth} = $_ unless $forms{$orth};
	my($morph) = (/\s#(\S+)/);
	if ($morph eq '~') {
	    my @post = ($curr_pos eq 'DN' || $curr_pos eq 'PN') ? @DN_PN_post : @default_post;
	    my $formcore = $_;
	    chomp $formcore;
	    $formcore =~ s/^\S+\s+//;
	    $formcore =~ s/\s*$//;
	    foreach my $post (@post) {
		my $f = "$orth-$post $formcore,$morph{$post}";
		$forms{"$orth-$post"} = $f;
	    }
	}
    } elsif (/^$acd_rx\@sense/ || /^\@end/) {
	my %seen = ();
	foreach my $f (sort keys %forms) {
	    print "\@form $forms{$f}\n" unless $seen{$f}++;
	}
	%forms = ();
	print;
    } else {
	print;
    }
}

1;
