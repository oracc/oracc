#!/usr/bin/perl
use warnings; use strict;

# Check that all of the texts in approved.lst ended up in the catalogue somehow

my %in_cat = ();

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
	s/^.*?://;
	s/\@.*$//;
	warn "01bld/lists/approved.lst:$.: $_ is not in the catalogue\n"
	    unless $in_cat{$_};
    }
    close(A);
}

1;
