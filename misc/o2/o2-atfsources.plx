#!/usr/bin/perl
use warnings; use strict;

my $bin = "$ENV{'ORACC'}/bin";
my @atf_sources = ();
my $atfsources = '01bld/atfsources.lst';

@atf_sources = <00atf/*.atf>;
push @atf_sources, <00atf/*.ods>;

chomp @atf_sources;
@atf_sources = ods_convert(@atf_sources);
if ($#atf_sources >= 0) {
    open(A,">$atfsources"); 
    print A join("\n", grep(/\.atf$/, @atf_sources)), "\n"; 
    close(A);
} else {
    unlink $atfsources;
    system 'touch', $atfsources;
}

sub
ods_convert {
    my @f = @_;
    my @n = ();
    my %seen = ();
    foreach my $f (@f) {
	if ($f =~ /\.ods$/) {
	    system "$bin/ods2atf.sh", $f;
	    push @n, "$f.atf" unless $seen{"$f.atf"}++;
	} else {
	    push @n, $f unless $seen{$f}++;
	}
    }
    @n;
}

1;
