#!/usr/bin/perl
use warnings; use strict;

my $listdir = '01bld/lists';
my $have_xtf = "$listdir/have-xtf.lst";
my $verbose = 1;

sub xsystem;

xtfindex_list();

######################################################################################

sub
xtfindex_list {
    my %proxy_projects = ();
    if (open(P,'01bld/lists/proxy-atf.lst')) {
	while (<P>) {
	    chomp;
	    s/:.*$//;
	    ++$proxy_projects{$_};
	}
	close(P);
	my @proxy_xtf = ();
	xsystem 'rm', '-f', '01bld/lists/proxy-xtf.lst';
	foreach my $p (keys %proxy_projects) {
	    push @proxy_xtf, '+?', "$ENV{'ORACC'}/bld/$p/lists/have-xtf.lst";
	}
	if ($#proxy_xtf >= 0) {
	    shift @proxy_xtf; # shift the leading '+?' off so the first list is lead
	    xsystem 
		'atflists.plx', '-o', "$listdir/proxy-xtf.lst",
		@proxy_xtf,
		'&','01bld/lists/proxy-atf.lst';
	}
    }
    if (-s $have_xtf) {
	xsystem 
	    'atflists.plx', '-o', "$listdir/xtfindex.lst",
	    $have_xtf,
	    '+?',"$listdir/proxy-xtf.lst";
    } elsif (-r "$listdir/proxy-xtf.lst") {
	xsystem 'cp', "$listdir/proxy-xtf.lst", "$listdir/xtfindex.lst";
    }
}

sub
xsystem {
    warn "xsystem @_\n" if $verbose;
    system @_;
}

1;
