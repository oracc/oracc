#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; binmode STDOUT, ':utf8';
binmode STDERR, ':utf8';
use Data::Dumper;
my %cofs = ();
open(C,'>01tmp/l2p1-cofs.sig');
open(S,'01tmp/l2p1-simple.sig');
my $fields = <S>;
# print C $fields; ### NO: simple cofs psus are cat'ed together so no @fields on cofs/psus
while (<S>) {
    if (/\!0x/) {
	chomp;
	my($pre,$key,$sig,$nth,$rank) = (/^(.*?):(.*?)=(.*?)\!0x0*(\d+)\t(\d+)$/);
	if ($pre) {
	    my $index = $nth - 1;
	    my $v = '';
	    my ($lang) = ($pre =~ /\%(.*?)$/);
	    if ($index) {
		$v = "$pre:=$sig";
	    } else {
		$v = "$pre:$key=$sig";
	    }
	    push @{${$cofs{"$lang\:$key"}}[$index]}, $v;
	} else {
	    warn "01tmp/l2p1-simple.sig:$.: (cof) syntax error: $_\n";
	}
    }
}
close(S);

foreach my $c (keys %cofs) {
    my @parts = @{$cofs{$c}};
    permute(@parts);
}

close(C);

######################################################################

# an array of arrays; the first contains the heads, the remainder
# tails.  We join the first two lists, then call recursively to 
# join additional ones.
sub
permute {
    my @parts = @_;
    my @newheads = ();
    foreach my $h (@{$parts[0]}) {
	foreach my $t (@{$parts[1]}) {
	    push @newheads, "$h\&\&$t";
	}
    }
    shift @parts; shift @parts;
    if ($#parts >= 0) {
	permute( [@newheads] , @parts);
    } else {
	foreach (@newheads) {
	    print C "$_\t0\n";
	}
    }
}

1;
