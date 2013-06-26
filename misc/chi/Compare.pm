package ORACC::CHI::Compare;
use warnings; use strict;
use lib '@@ORACC@@/lib';
use ORACC::CHI::Sorter;

sub
chi2s_not_in_chi1 {
    my($chi1index, $chi2) = @_;
    my @bad = ();
    foreach my $c (keys %$chi2) {
	next if $c eq '@prefixes';
	push(@bad, $c) unless $$chi1index{$c};
    }
    return @bad;
}

sub
ignoring_joins {
    my($set_a, $set_b) = (@_);
    my $ij_set_a = simplify($set_a);
    my $ij_set_b = simplify($set_b);
    $ij_set_a cmp $ij_set_b;
}

sub
simplify {
    my $set = shift;
    $set =~ s/\.+-?/./g;
    ORACC::CHI::Sorter::sortuniq_direct($set);
}

1;
