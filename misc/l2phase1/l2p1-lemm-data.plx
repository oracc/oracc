#!/usr/bin/perl
use warnings; use strict; use open ':utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Getopt::Long;


my %f = ();
my $freq = 0;
my %lemmdata = ();
my $rank = 0;
my $update = 0;

GetOptions (
    'update'=>\$update
    );

if ($update) {
    update();
} else {
    rebuild();
}

########################################################################################

sub rebuild {
    my $f = "01bld/project.sig";
 
    die "l2p1-lemm-data.plx: no signatures file $f\n"
	unless (open(F, $f));
    while (<F>) {
	next if /^\@(?:project|name|lang)/ || /^\s*$/;
	chomp;
	if (/^\@fields/) {
	    set_f($_);
	    next;
	}
	my @fields = split(/\t/,$_);
	my $sig = $fields[0]."\t$fields[$rank]\t$fields[$freq]\n";
	my ($lng) = ($sig =~ /\%(.*?):/);
	$lng =~ s/-\d\d\d//; # remove script codes
	push @{$lemmdata{$lng}}, $sig;
    }
    close(F);    
    
    dump_lemm();
}

sub update {
    my @f = `ls 01bld/*/from_glo.sig`; chomp @f; @f = grep /\.sig$/, @f;

    die "l2p1-lemm-data.plx: no signatures files 01bld/*/from_glo.sig\n"
	unless $#f >= 0;
    
    my %freqs = ();
    my %ranks = ();
    my %sigs = ();

    # Harvest sigs and ranks from the project's glossaries
    foreach my $f (@f) {
	die "l2p1-lemm-data.plx: no signatures file $f\n"
	    unless (open(F, $f));
	warn "reading $f ...\n";
	while (<F>) {
	    next if /^\@(?:project|name|lang)/ || /^\s*$/;
	    chomp;
	    if (/^\@fields/) {
		set_f($_, qw/sig rank/);
		next;
	    }
	    my @fields = split(/\t/,$_);
	    ++$sigs{$fields[0]};
	    $ranks{$fields[0]} = $fields[$rank];
	}
	close(F);
    }
    
    # Harvest freqs from the last set of lemm-data
    foreach my $l (<02pub/lemm-*.sig>) {
	open(L,$l) || die "l2p1-lemm-data.plx: can't open `$l' for update\n";
	while (<L>) {
	    next if /^\s*/ || m/^\@(?:project|name|lang)/;
	    chomp;
	    if (/^\@fields/) {
		set_f($_, qw/sig freq/);
		next;
	    }
	    my @fields = split(/\t/,$_);
	    $freqs{$fields[0]} = $fields[$freq];
	}
	close(L);
    }

    # Add freqs to sigs/ranks and dump new lemm-data. If a project
    # is using dynamic lemm-data from proxies that proxy data will 
    # not make it into this version. This is a limitation of update.
    foreach my $s (keys %sigs) {
	my ($r,$f) = (0,0);
	$r = $ranks{$s} if $ranks{$s};
	$f = $freqs{$s} if $freqs{$s};
	my ($lng) = ($s =~ /\%(.*?):/);
	$lng =~ s/-\d\d\d//; # remove script codes
	$s = "$s\t$r\t$f\n";
	push @{$lemmdata{$lng}}, $s;
    }

    dump_lemm();
}

sub byfreq {
    my ($af) = ($a =~ /(\d+)$/);
    my ($bf) = ($b =~ /(\d+)$/);
    $bf <=> $af;
}

sub dump_lemm {
    foreach my $l (keys %lemmdata) {
	open(L, ">02pub/lemm-$l.sig") 
	    || die "l2p1-lemm-data.plx: Strange, can't write 02pub/lemm-$l.sig. Stop";
	print L "\@fields sig rank freq\n";
	print L sort { &byfreq }  @{$lemmdata{$l}};
	close L;
    }
}

sub set_f {
    my ($fields,@required) = @_;
    %f = ();
    my @f = split(/\s/, $fields);
    shift @f; # drop '@field';
    for (my $i = 0; $i <= $#f; ++$i) {
	$f{$f[$i]} = $i;
    }

    if ($#required >= 0) {
	foreach my $rq (@required) {
	    die "l2p1-lemm-data.plx: can't rebuild  without all of @required. Stop\n"
		unless exists $f{$rq};
	}
    }
 
    $rank = $f{'rank'};
    $freq = $f{'freq'};
}

1;
