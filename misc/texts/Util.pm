package ORACC::Texts::Util; require Exporter; @ISA=qw/Exporter/;

@EXPORT = qw/wid2lem_load wid2lem_inst wid2lem_by_sig wid2lem_sigs wid2lem_loc/;

use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;

use Data::Dumper;

my $w2l_trace = 0;

my $do_w2l_index = 1;
my %wid_map = ();
my %w2l_data = ();
my %w2l_index = ();
my %w2l_langs = ();

sub wid2lem_loc {
    $wid_map{$_[0]};
}

sub wid2lem_sigs {
    $do_w2l_index = 0;
    wid2lem_load(@_);
    %w2l_data;
}

sub wid2lem_by_sig {
    my ($w2l_err,$wid2lem,$sig_err,$sigs,$warn_not_found) = @_;
    wid2lem_load($w2l_err, $wid2lem);
    my $i = 0;
    my %ok = ();
    foreach my $s (@$sigs) {
	++$i;
	next if $s =~ /^\s*$/; # blank lines allowed to maintain line numbers as in source file for sigs
	my %p = parse_sig($s);
	if ($p{'cf'} && $p{'gw'} && $p{'pos'}) {
	    my $cfgwpos = "$p{'cf'}\[$p{'gw'}\]$p{'pos'}";
	    warn "W2L: resolving sig $s via cfgw $cfgwpos\n" if $w2l_trace;
	    if ($w2l_index{$cfgwpos}) {
#		if ($p{'lang'}) { ## drop this becuase lang is not used in key of w2l_index
		    my @cand = @{$w2l_index{$cfgwpos}};
		    my @ok = ();
		    if ($p{'base'}) {
			$p{'base'} =~ s/^\%.*?://;
		    }
		    foreach my $c (@cand) {
		      candidate:
			{
			    warn "W2L: matching sig $s against cand $c\n" if $w2l_trace;
			    foreach my $k (keys %p) {
				next if $k =~ /^#/;
				warn "W2L: testing $k: $p{$k} vs. $$c{$k}\n" if $w2l_trace;
				next candidate unless $p{$k} eq $$c{$k};
			    }
			    my $sig = $$c{'#sig'};
			    if ($sig) {
				foreach my $d (@{$w2l_data{$sig}}) {
				    my ($wid,$inst) = @$d;
				    if ($wid_map{$wid}) {
					warn "push \@ok, [ @{$wid_map{$wid}} , $wid , $inst , $sig ]\n" if $w2l_trace;
					push @ok, [ @{$wid_map{$wid}} , $wid , $inst , $sig ];
				    } else {
					warn "$0: internal error: wid $wid not in wid_map";
				    }
				}
			    } else {
				warn "$0: sig not in w2l_data\n";
				warn Dumper $c;
			    }
			}
		    }
		    ### now @ok is all the matches to this sig in wid2lem
		    if ($#ok >= 0) {
			push @{$ok{$s}}, @ok;
		    } else {
			warn "$0: $s not found in lemmatized corpus\n";
		    }
#		} else {
#		    warn "$sig_err:$i: no \%lang in sig $s\n";
#		}
	    } else {
		warn "$sig_err:$i: $cfgwpos not found in wid2lem input\n" if $warn_not_found;
	    }
	} else {
	    warn "$sig_err:$i: no CF[GW]POS found in sig $s\n";
	}
    }
    # print Dumper \%ok; exit 0;
    %ok;
}

sub wid2lem_inst {
    my ($w2l_err,$wid2lem) = @_;
    my $i = 0;
    my %seen = ();
    foreach my $w2l (@$wid2lem) {
	++$i;
	my($f1,$f2,$f3) = split(/\t/, $w2l);
	if ($f3 && $f3 =~ /^\d+$/) { # it's a WID FILE LINE triple
	    $wid_map{$f1} = [ $f2 , $f3 ];
	} elsif ($f3 && $f3 =~ /^\@/) { # it's a WID INSTANCE SIGNATURE triple
#	    push @{$w2l_data{$f3}}, [ $f1 , $f2 ];
	    unless ($seen{$f3}++) {
		my %p = parse_sig($f3);
		if ($p{'cf'} && $p{'gw'} && $p{'pos'}) {
		    $p{'#sig'} = $f3;
		    if ($p{'lang'} =~ /^sux/) {
			$p{'base'} =~ s/^\%.*?://;
		    }
		    $p{'form'} =~ s/^\%.*?://;
		    # push @{$w2l_index{"$p{'cf'}\[$p{'gw'}\]$p{'pos'}"}}, { %p };
		    $w2l_index{"$p{'form'}=$f2"} = $f3;
		} else {
		    warn "$w2l_err:$i: no CF[GW]POS found in sig $f3\n";
		}
	    }
	} else {
	    if ($f3) {
		warn "$w2l_err:$i: third field is not a line number or signature (f3=$f3)\n"
		    unless $f3 eq 'BAD';
	    } # else it's a WID and unlemmatized token entry
	}
    }
#    open(W,'>w2l_data.dump'); print W Dumper \%w2l_data; close(W);
    open(W,'>w2l_index.dump'); print W Dumper \%w2l_index; close(W);
    %w2l_index;
}


sub wid2lem_load {
    my ($w2l_err,$wid2lem) = @_;
    my @wid2lem = ();
    unless ($wid2lem) {
	@wid2lem = `cat $w2l_err`;
	chomp @wid2lem;
	$wid2lem = \@wid2lem;
    }
    my $i = 0;
    my %seen = ();
    foreach my $w2l (@$wid2lem) {
	++$i;
	my($f1,$f2,$f3) = split(/\t/, $w2l);
	if ($f3 && $f3 =~ /^\d+$/) { # it's a WID FILE LINE triple
	    $wid_map{$f1} = [ $f2 , $f3 ];
	} elsif ($f3 && $f3 =~ /^\@/) { # it's a WID INSTANCE SIGNATURE triple
	    push @{$w2l_data{$f3}}, [ $f1 , $f2 ];
	    if ($do_w2l_index) {
		unless ($seen{$f3}++) {
		    my %p = parse_sig($f3);
		    if ($p{'cf'} && $p{'gw'} && $p{'pos'}) {
			++$w2l_langs{$p{'lang'}};
			$p{'#sig'} = $f3;
			if ($p{'lang'} =~ /^sux/) {
			    $p{'base'} =~ s/^\%.*?://;
			}
			$p{'form'} =~ s/^\%.*?://;
			push @{$w2l_index{"$p{'cf'}\[$p{'gw'}\]$p{'pos'}"}}, { %p };
		    } else {
			warn "$w2l_err:$i: no CF[GW]POS found in sig $f3\n";
		    }
		}
	    }
	} else {
	    if ($f3) {
		warn "$w2l_err:$i: third field is not a line number or signature (f3=$f3)\n"
		    unless $f3 eq 'BAD';
	    } # else it's a WID and unlemmatized token entry
	}
    }
    open(W,'>w2l_data.dump'); print W Dumper \%w2l_data; close(W);
    open(W,'>w2l_index.dump'); print W Dumper \%w2l_index; close(W);
}

1;
