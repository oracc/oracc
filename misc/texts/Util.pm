package ORACC::Texts::Util; require Exporter; @ISA=qw/Exporter/;

@EXPORT = qw/wid2lem_load wid2lem_by_sig/;

use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;

use Data::Dumper;

my %w2l_index = ();
my %w2l_data = ();

sub wid2lem_by_sig {
    my ($w2l_err,$wid2lem,$sig_err,$sigs,$warn_not_found) = @_;
    wid2lem_load($w2l_err, $wid2lem);
    my $i = 0;
    my %ok = ();
    foreach my $s (@_) {
	++$i;
	next if $s =~ /^\s*$/; # blank lines allowed to maintain line numbers as in source file for sigs
	my %p = parse_sig($s);
	if ($p{'cf'} && $p{'gw'} && $p{'pos'}) {
	    my $cfgwpos = "$p{'cf'}\[$p{'gw'}\]$p{'pos'}";
	    if ($w2l_index{$cfgwpos}) {
		if ($p{'lang'}) {
		    my @cand = @{$w2l_index{$cfgwpos}};
		    my @ok = ();
		  candidate:
		    {
			foreach my $c (@cand) {
			    foreach my $k (keys %p) {
				next if $k =~ /^#/;
				next candidate unless $p{$k} eq $$c{$k};
			    }
			    my $sig = $$c{'sig'};
			    my $data = $w2l_data{$sig};
			    my $wid = $$data[0];
			    my $inst = $$data[1];
			    if ($w2l_map{$wid}) {
				push @ok, [ @$w2l_map{$wid} , $wid , $inst , $sig ];
			    } else {
				warn "$0: internal error: wid $wid not in w2l_map";
			    }			      
			}
		    }
		    ### now @ok is all the matches to this sig in wid2lem
		    push @{$ok{$s}}, @ok;
		} else {
		    warn "$sig_err:$i: no \%lang in sig $s\n";
		}
	    } else {
		warn "$sig_err:$i: $cfgwpos not found in wid2lem input\n" if $warn_not_found;
	    }
	} else {
	    warn "$sig_err:$i: no CF[GW]POS found in sig $s\n";
	}
    }
    print Dumper \%ok; exit 0;
}

sub wid2lem_load {
    my ($w2l_err,$wid2lem) = @_;
    my %wid_map = ();
    my $i = 0;
    foreach my $w2l (@$wid2lem) {
	++$i;
	my($f1,$f2,$f3) = split(/\t/, $w2l);
	if ($f3 && $f3 =~ /^\d+$/) { # it's a WID FILE LINE triple
	    $wid_map{$f1} = [ $f2 , $f3 ];
	} elsif ($f3 && $f3 =~ /^\@/) { # it's a WID INSTANCE SIGNATURE triple
	    push @{$w2l_data{$f3}}, [ $f1 , $f2 ];
	    my %p = parse_sig($f3);
	    if ($p{'cf'} && $p{'gw'} && $p{'pos'}) {
		$p{'#sig'} = $f3;
		$p{'base'} =~ s/^\%.*?://
		$p{'form'} =~ s/^\%.*?://
		push @{$w2l_index{"$p{'cf'}\[$p{'gw'}\]$p{'pos'}"}}, { %p };
	    } else {
		warn "$w2l_err:$i: no CF[GW]POS found in sig $f3\n";
	    }
	} else {
	    if ($f3) {
		warn "$w2l_err:$i: third field is not a line number or signature\n";
	    } # else it's a WID and unlemmatized token entry
	}
    }
    print Dumper \%w2l_data;
    print Dumper \%w2l_index;
}

1;
