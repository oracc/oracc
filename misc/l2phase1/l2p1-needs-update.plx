#!/usr/bin/perl
use warnings; use strict;

my $g2 = `oraccopt . g2`;

my $from_glos_sig = '01bld/from-glos.sig';
my $sig_date = (stat($from_glos_sig))[9];
my $status = 1; # default to not needing update

if ($g2 eq 'yes') {
    my $g2_verb = 1;
    foreach my $g (<01tmp/*.glo>) {
	my $glo_date = undef;
	my $lib = $g; $lib =~ s/01tmp/00lib/;
	my $src = $g; $src =~ s/01tmp/00src/;
	$glo_date = (stat($g))[9];
	my $src_date = (stat($lib))[9];
	my $gsrc = '';
	unless (defined $src_date) {
	    $src_date = (stat($lib))[9];
	    if (!defined($src_date)) {
		warn "l2p1-needs-update.plx: weird: no $lib or $src for $g\n";
		continue;
	    } else {
		$gsrc = $lib;
	    }
	} else {
	    $gsrc = $src;
	}
	if ($glo_date < $src_date) {
	    warn "l2p1-needs-update.plx: updating $g older than $gsrc\n"
		if $g2_verb;
	    system 'cbdpp.plx', $g;
	    $glo_date = (stat($g))[9];
	    $status = 0;
	    next;
	} else {
	    warn "l2p1-needs-update.plx: $g newer than $gsrc\n"
		if $g2_verb;
	}
	if (!-z $g
	    && (!defined($sig_date) || $glo_date > $sig_date)) {
	    $status = 0;
	    warn "l2p1-needs-update.plx: $g newer than $from_glos_sig, update needed\n"
		if $g2_verb;
	} else {
	    warn "l2p1-needs-update.plx: $from_glos_sig newer than $g, no update needed\n"
		if $g2_verb;
	}
    }
} else {
    foreach my $g (<00lib/*.glo>) {
	my $glo_date = undef;
	if ($g =~ m#/(?:sux|qpn)#) {
	    my $norm = "$g.norm";
	    $norm =~ s/00lib/01bld/;
	    $glo_date = (stat($g))[9];
	    my $norm_date = (stat($norm))[9];
	    if (!defined($norm_date) || $glo_date > $norm_date) {
		system 'l2p1-sux-norm.plx', $g;
		$status = 0;
		next;
	    } else {
		$g = $norm;
		$glo_date = (stat($g))[9];
	    }
	} else {
	    $glo_date = (stat($g))[9];
	}
	if (!-z $g
	    && (!defined($sig_date) || $glo_date > $sig_date)) {
	    $status = 0;
	}
    }
}

exit($status);
