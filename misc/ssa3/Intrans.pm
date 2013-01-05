package ORACC::SSA3::Intrans;
use warnings; use strict; use open ':utf8';
use Data::Dumper;

my %intrans = ();
my $intrans_loaded = 0;

sub
load_intrans {
    return if $intrans_loaded++;
    open(IN,'@@ORACC@@/lib/ORACC/SSA2/data/intrans.lst')
	|| die("Intrans: can't find intrans.lst");
    while (<IN>) {
	chomp;
	$intrans{$_} = 1;
    }
    close(IN);
}

sub
is_intrans {
    my $p = shift;
    $$p{'pos'} =~ /i$/; # L2-specific: epos = V/i is intrans; epos = V/t = trans
#    my $cfgw = $$p{'pos'};
#    warn Dumper $p unless $cfgw;
#    $cfgw =~ s/\].*$/]/;
#    print STDERR "testing '$cfgw' for intransitivity...", 
#                  $intrans{$cfgw} ? 'yes' : 'no', "\n";
#    $intrans{$cfgw};
}

1;
