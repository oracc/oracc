#!/usr/bin/perl
use warnings; use strict;

my @lists = ('01bld/lists/proxy-atf.lst',
	     '01bld/lists/proxy-cat.lst',
	     '01bld/lists/proxy-lem.lst'
    );
my %cat = ();
my @cat = ();
open(C, '01bld/lists/cat-ids.lst'); @cat = (<C>); chomp @cat;
@cat{@cat} = ();
foreach my $l (@lists) {
    open(L, $l);
    my @l = (<L>); chomp @l;
    close(L);
    open(L, ">$l");
    foreach my $id (@l) {
	my ($pqx) = ($id =~ /([PQX]\d\d\d\d\d\d)/);
#	warn "testing $pqx\n";
	if (exists $cat{$pqx}) {
	    print L "$id\n";
	} else {
	    warn "pruning $pqx from $l\n";
	}
    }
    close(L);
}

1;
