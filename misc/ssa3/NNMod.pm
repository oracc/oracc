package ORACC::SSA3::NNMod;
use warnings; use strict; use open ':utf8';

my $nnmod_debug = 0;
my %post_mod = ();
my %pre_mod = ();
my $loaded = 0;

# take a pair of CFGW pairs and look them up in the nnmod database
# return 1 for post-mod
# return -1 for pre-mod
# return 0 for non-match
sub
is_nnmod {
    load_nnmod() unless $loaded;
    my ($n1,$n2) = @_;
    return 0 unless $n1 && $n2;
    print STDERR "NNMod: testing '$n1' '$n2'\n" if $nnmod_debug;
    if ($post_mod{$n1}) {
	if (${$post_mod{$n1}}{$n2}) {
	    return 1;
        }
    } elsif ($pre_mod{$n1}) {
	if (${$pre_mod{$n1}}{$n2}) {
	    return -1;
        }
    }
    0;
}

sub
load_nnmod {
    $loaded = 1;
#    open(IN,'@@ORACC@@/lib/ORACC/SSA3/data/nnmod.lst')
#	|| die("NNMod: can't find nnmod.lst");
    *DATA = ORACC::SSA3::Util::open_data('nnmod.lst');
    while (<DATA>) {
	next if /^\s*\#/ || /^\s*$/;
	s/\s*$//;
	s/^(.*?\[.*?\])\s*//;
	my $a = $1;
	my $b = $_;
	if ($a =~ s/^\+//) {
	    ${$pre_mod{$a}}{$b} = 1;
	} else {
	    ${$post_mod{$a}}{$b} = 1;
	}
    }
    close(DATA);
}

1;
