#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
my $currpqx = '';
my %rels = ();
my %symtab = ();

my $project = `oraccopt`; exit 0 if $project eq "cdli";

open(S,$ARGV[0]);

while (<S>) {
    chomp;
    my($pqx,$rel,$lnk) = split(/\t/,$_);
    if ($pqx ne $currpqx) {
	%symtab = ();
	$currpqx = $pqx;
    }
    $lnk =~ s/\s*=.*$//;
    if ($rel =~ /^def\s+(\S+)\s*$/) {
	$symtab{$1} = $lnk;
    } elsif ($rel =~ /^>>\s*(\S+)/) {
	if ($symtab{$1}) {
	    ++$rels{"$symtab{$1}\thas-witness\t$currpqx\n"};
#	    ++$rels{"$currpqx\tis-witness-of\t$symtab{$1}\n"};
	} else {
	    warn "$currpqx: no definition for $1\n";
	}
    } elsif ($rel =~ /^<<\s*(\S+)/) {
	if ($symtab{$1}) {
#	    ++$rels{"$symtab{$1}\tis-witness-of\t$currpqx\n"};
	    ++$rels{"$currpqx\thas-witness\t$symtab{$1}\n"};
	} else {
	    warn "$currpqx: no definition for $1\n";
	}
    } elsif ($rel =~ /^\|\|\s*(\S+)/) {
	if ($symtab{$1}) {
	    ++$rels{"$symtab{$1}\tparallels\t$currpqx\n"};
#	    ++$rels{"$currpqx\tparallels\t$symtab{$1}\n"};
	} else {
	    warn "$currpqx: no definition for $1\n";
	}
    } elsif ($rel eq 'parallel') {
	++$rels{"$lnk\tparallels\t$currpqx\n"};
	++$rels{"$currpqx\tparallels\t$lnk\n"};
    } elsif ($rel eq 'source') {
	++$rels{"$currpqx\thas-witness\t$lnk\n"};
	++$rels{"$lnk\tis-witness-of\t$currpqx\n"};
    } else {
	warn "$_\n";
    }
}

print sort keys %rels;

1;
