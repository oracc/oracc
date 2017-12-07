#!/usr/bin/perl
use warnings; use strict;

# Check that all of the texts in approved.lst ended up in the catalogue somehow

my $project = `oraccopt`;
my %in_cat = ();
my %seen = ();

if (-r '02pub/cat/pqids.lst') {
    open(C,'02pub/cat/pqids.lst');
    while (<C>) {
	chomp;
	s/^.*://;
	s/\@.*$//;
	++$in_cat{$_};
    }
    close(C);
}

if (-r '01bld/lists/approved.lst') {
    open(A,'01bld/lists/approved.lst');
    while (<A>) {
	chomp;
	my $xmd_project = '';
	my $xtf_project = '';
	s/^(.*?):// && ($xtf_project = $1);
	s/\@(.*)$// && ($xmd_project = $1);
	unless ($in_cat{$_}) {
	    if ($xtf_project eq $project) {
		warn "01bld/lists/approved.lst:$.: $_ is not in the catalogue\n";
		++$seen{$_};
	    }
	}
    }
    close(A);
}

if (-r '01bld/lists/xtfindex.lst') {
    open(A,'01bld/lists/xtfindex.lst');
    while (<A>) {
	chomp;
	my $xmd_project = '';
	my $xtf_project = '';
	s/^(.*?):// && ($xtf_project = $1);
	s/\@(.*)$// && ($xmd_project = $1);
	unless ($in_cat{$_}) {
	    if ($xtf_project eq $project) {
		warn "01bld/lists/xtfindex.lst:$.: $_ is not in the catalogue\n";
		++$seen{$_};
	    }
	}
    }
    close(A);
}

if (-r '01bld/lists/proxy-atf.lst') {
    open(A,'01bld/lists/proxy-atf.lst');
    while (<A>) {
	chomp;
	my $xmd_project = '';
	my $xtf_project = '';
	my $fullname = $_;
	s/^(.*?):// && ($xtf_project = $1);
	s/\@(.*)$// && ($xmd_project = $1);
	unless ($in_cat{$_}) {
	    warn "01bld/lists/proxy-atf.lst:$.: $fullname is not in the $xmd_project catalogue\n";
	    ++$seen{$_};
	}
    }
    close(A);
}

1;
