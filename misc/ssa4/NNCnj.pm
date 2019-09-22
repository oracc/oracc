package ORACC::SSA4::NNCnj;
use warnings; use strict; use open ':utf8';

my %cnj = ();
my $loaded = 0;

# take a pair of CFGW pairs and look them up in the nncnj list
# return 1 for match
# return 0 for non-match
sub
is_nncnj {
    load_nncnj() unless $loaded;
    my ($n1,$n2) = @_;
#    print STDERR "n1=$n1; n2=$n2\n";
    $n1 && $n2 && $cnj{$n1} && ${$cnj{$n1}}{$n2};
}

sub
load_nncnj {
    $loaded = 1;
    open(IN,'@@ORACC@@/lib/ORACC/SSA4/data/nncnj.lst') 
	|| die("NNCnj: can't find nncnj.lst");
    while (<IN>) {
	next if /^\s*\#/ || /^\s*$/;
	s/\s*$//;
	s/^(.*?(?:\[.*?\])?)\s+//;
	my $a = $1;
	my $b = $_;
#	print STDERR "nncnj: $a ++ $b\n";
	${$cnj{$a}}{$b} = 1;
    }
    close(IN);
}

1;
