package ORACC::CHI::Writer;
use warnings; use strict;

sub
write {
    my ($chiref,$output) = @_;
    unless ($output eq '-') {
	open(OUT, ">$output") || die "ORACC::CHI::Serializer: can't write to $output\n";
	select(OUT);
    }
    foreach my $c (sort keys %{$chiref}) {
	my %cdata = %{$$chiref{$c}};
	if ($cdata{'_terminal'}) {
	    print "$c\t_terminal\t$cdata{'_terminal'}\n";
	}
	foreach my $rel (sort keys %cdata) {
	    next if $rel eq '_terminal';
	    foreach my $elt (@{$cdata{$rel}}) {
		print "$c\t$rel\t$elt\n";
	    }
	}
    }
    close(OUT) unless $output eq '-';
}

1;
