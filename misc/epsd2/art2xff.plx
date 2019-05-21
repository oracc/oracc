#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::NS;
use ORACC::L2GLO::Util;
use Getopt::Long;
use Data::Dumper;

my @fields = qw/base cont form morph/; # norm needs fixing
my $file = '';
my @forms = ();
my $lang = '';
my $xffdir = '';

GetOptions(
    'dir:s'=>\$xffdir,
    'file:s'=>\$file,
    'lang:s'=>\$lang,
    );

die "art2xff.plx: must give language on command line (e.g., art2xff.plx -lang sux)\n"
    unless $lang;

$file = "01bld/$lang/articles.xml" unless $file;

my $art = load_xml($file);
foreach my $entry (tags($art,$CBD,'entry')) {
    my $icount = $entry->getAttribute('icount');
    next if $icount eq '-1' || $icount eq '0';
    my %fields = index_fields($entry);
    my %e = ();
    $e{'cfgw'} = $entry->getAttribute('n');
    $e{'id'} = xid($entry);
    $e{'freq'} = $entry->getAttribute('icount');
    $e{'xis'} = $entry->getAttribute('xis');
    foreach my $sig (tags($entry,$CBD,'sig')) {
	my $icount = $sig->getAttribute('icount');
	next if $icount eq '-1' || $icount eq '0';
	my %sig = ();
	$sig{'id'} = xid($sig);
	$sig{'icount'} = $sig->getAttribute('icount');
	$sig{'ipct'} = $sig->getAttribute('ipct');
	$sig{'xis'} = $sig->getAttribute('xis');
	my $sigfromattr = $sig->getAttribute('sig');
	if ($sigfromattr =~ /^\{/) {
	    $sig{'parsed'} = { ORACC::L2GLO::Util::parse_psu($sigfromattr) };
	    my @p = @{@{$sig{'parsed'}}{'part_sigs'}};
#	    warn Dumper \@p;
	    my $f = '';
	    foreach my $p (@p) {
#		warn "$p\n";
		$f .= ' ' if $f;
		$p =~ /\%.*?:(.*?)=/;
		$f .= $1;
	    }
	    ${$sig{'parsed'}}{'form'} = $f;
	} else {
	    $sig{'parsed'} = { ORACC::L2GLO::Util::parse_sig($sigfromattr) };
	}
#	print Dumper \%sig;
	foreach my $f (@fields) {
	    my $sig_v = ${$sig{'parsed'}}{$f};
	    if ($sig_v && $fields{$f}) {
		my $sig_v_no_lang = $sig_v;
		$sig_v_no_lang =~ s/^\%.*?://;
		if (${$fields{$f}}{$sig_v_no_lang}) {
		    my $f_data = ${$fields{$f}}{$sig_v_no_lang};
		    ${$sig{'parsed'}}{$f} = $f_data;
		}
	    }
	}
	push @{$e{'forms'}}, { %sig };
	%sig = ();
    }
    push @forms, { %e };
    %fields = ();
}

dump_xff(@forms);

#######################################################################################

sub
dump_xff {
    system 'mkdir', '-p', $xffdir;
    foreach my $f (@_) {
	my %e = %$f;
	open(E, ">$xffdir/$e{'id'}.xff"); select E;
	print '<?xml version="1.0" encoding="utf-8"?>', "\n";
	print "<sigs xmlns=\"http://oracc.org/ns/xff/1.0\" xmlns:xff=\"http://oracc.org/ns/xff/1.0\" xml:lang=\"$lang\" sortopt=\"freq\" reverse=\"yes\" eid=\"$e{'id'}\" formset=\"$e{'cfgw'}\">";
	foreach my $s (@{$e{'forms'}}) {
	    my %sig = %$s;
	    print "<sig xml:id=\"$sig{'id'}\" icount=\"$sig{'icount'}\" ipct=\"$sig{'ipct'}\" xis=\"$sig{'xis'}\">";
	    foreach my $f (@fields) {
		my $v = ${$sig{'parsed'}}{$f} || '';
		if ($f eq 'base') {
		    $$v[0] =~ s/^\%.*?://;
		    $$v[0] =~ s/\s+\%.*?:/ /;
		}
		if (ref($v) eq 'ARRAY') {
		    print "<$f n=\"$$v[0]\" icount=\"$$v[1]\" ipct=\"$$v[2]\" xis=\"$$v[3]\"/>";
		} else {
		    print "<$f/>";
		}
	    }
	    print '</sig>';
	}
	print '</sigs>';
	close(E);
    }
}

sub
index_fields {
    my $e = shift;
    my %f = ();
    foreach my $f (@fields) {
	foreach my $i (tags($e,$CBD,$f)) {
	    my $icount = $i->getAttribute('icount');
	    next if $icount eq '0' || $icount eq '-1';
	    my $v = $i->getAttribute('n');
	    warn "no v for ", $i->toString(), "\n"
		unless $v;
	    my @data = (
		$v ,
		$icount ,
		$i->getAttribute('ipct') ,
		$i->getAttribute('xis') 
		);
	    ${$f{$f}}{$v} = [ @data ];
	}
    }
    %f;
}

1;
