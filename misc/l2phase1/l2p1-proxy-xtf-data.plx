#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;
use Getopt::Long;
use Data::Dumper;

my $master = '';

GetOptions(
    'master:s'=>\$master,
    );

$master = '01bld/from-glos.sig' unless $master;

my %m = ();

open(M, $master)
    || die "l2p1-proxy-xtf-data.plx: can't open master sigs $master\n";
while (<M>) {
    chomp;
    my %s = ORACC::L2GLO::Util::parse_sig($_);
    my $cfgwpos = "$s{'cf'}\[$s{'gw'}\]$s{'pos'}";
    my $snseposfrmnrm = "$s{'form'}\:$s{'sense'}\:$s{'epos'}"; #\:$s{'norm'}
    ${$m{$cfgwpos}}{ $snseposfrmnrm } = { sig=>$_ };
}
close(M);

#print Dumper(\%m);

open(F, '00lib/from-xtf.lst') 
    || die "l2p1-proxy-xtf-data.plx: can't open 00lib/from-xtf.lst\n";
while (<F>) {
    chomp;
    my $fuzzy_forms = 1;
    my $project = $_;
    open(X, "$ENV{'ORACC'}/bld/$project/from-xtf-glo.sig")
	|| die "l2p1-proxy-xtf-data.plx: can't open $ENV{'ORACC'}/$project/from-xtf-glo.sig\n";
    while (<X>) {
	chomp;
	my($sig,$rest) = ();
	if (/^(.*?)\t(.*?)$/) {
	    ($sig,$rest) = ($1,$2);
	    my %s = ORACC::L2GLO::Util::parse_sig($_);
	    $s{'form'} =~ s/\\.*$// if $fuzzy_forms;	    
	    my $cfgwpos = "$s{'cf'}\[$s{'gw'}\]$s{'pos'}";
	    my $snseposfrmnrm = "$s{'form'}\:$s{'sense'}\:$s{'epos'}"; #\:$s{'norm'}
	    if ($m{$cfgwpos}) {
		if (${$m{$cfgwpos}}{ $snseposfrmnrm }) {
		    $rest =~ s/^.*?\t//;
		    push @{${$m{$cfgwpos}}{$snseposfrmnrm}{'refs'}}, $rest;
		}
	    } else {
		
	    }
	}
    }
    close(X);
}
close(F);

open(G, '>01bld/from-proxy-xtf.sig')
    || die "l2p1-proxy-xtf-data.plx: can't write to 01bld/from-proxy-xtf.sig\n";
foreach my $cfgw (keys %m) {
    foreach my $form (keys %{$m{$cfgw}}) {
	my %r = ();
	foreach my $refs (@{${$m{$cfgw}}{$form}{'refs'}}) {
	    foreach my $r (split(/\s+/, $refs)) {
		my ($pqx) = ($r =~ /:(.*)$/);
		$r{$pqx} = $r unless $r{$pqx};
	    }
	}
	print G ${${$m{$cfgw}}{$form}}{'sig'}, "\t", join(' ', keys %r), "\n";
    }
}

1;
