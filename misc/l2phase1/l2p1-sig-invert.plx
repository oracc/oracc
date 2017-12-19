#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;

use Data::Dumper;

my @fields = qw/lang form cf gw sense pos epos norm base cont morph1 morph1/;
my @insts = ();
my %keyvals = ();
my @pool = ();
my %sortcodes = ();
my $state = 0; # 0 = pre-stringpool; 1 = stringpool
my %tab = ();
my @texts = ();

open(T, '01bld/sortinfo.tab');
while (<T>) {
    $state = 1 if /^\#nstring/;
    next if /^\#/;
    if ($state) {
	@pool = split(/\000/,$_);
    } else {
	my($pqid,$genre,$place,$time,$name) = split(/\t/,$_);
	$pqid =~ s/^P2(\d\d\d\d\d\d)$/X$1/;
	push @texts, [ $pqid, $genre, $place, $time, $name ];
    }
}
close(T);

foreach my $t (@texts) {
    my($pqid,$genre,$place,$time,$name) = @$t;
    my $tab = "$pqid";
    foreach my $f ($name,$genre,$place,$time) {
	my($sort,$str) = ($f =~ /^(.*?)=(.*?)$/);
	$tab .= "\t$pool[$str]\t$sort";
    }
    $tab{$pqid} = $tab;
}

unless (-r '01bld/project.sig') {
    system 'l2p1-project-from-union.sh';
}

open(P, '01bld/project.sig');
while (<P>) {
    chomp;
    next if /^\@fields/;
    my($sig,$count,$insts) = split(/\t/);
    $sig =~ s/\&\&.*$//; ### FIXME: must handle COFs and PSUs properly
    my %sig = parse_sig($sig);
    $sig{'form'} =~ s/^\%.*?://;
    my @inst = expand_insts($insts);
    foreach my $f (@fields) {
	if ($sig{$f}) {
	    ++${$keyvals{$f}}{$sig{$f}};
	}
    }    
    foreach my $i (@inst) {
	push @insts, [ $i , \%sig ];
    }
}
close(P);

#use Data::Dumper; open(K,'>keyvals.dump'); print K Dumper \%keyvals; close(K);

foreach my $f (@fields) {
    my @vals = keys %{$keyvals{$f}};
    next unless $#vals > 0;
    my $cgc_ref = set_cgc(@vals);
    my %cgc = %$cgc_ref;
    foreach my $v (@vals) {
	${$sortcodes{$f}}{$v} = $cgc{$v};
    }
}

open(I,'>01bld/inverted.sig'); select I;
foreach my $inst (@insts) {
    my($i,$sigref) = @$inst;
    my %sig = %$sigref;
    print "$sig{'lang'}";
    foreach my $f (@fields[1..$#fields]) { # skip lang
	if ($sig{$f}) {
	    my $code = 0;
	    if ($sortcodes{$f} && ${$sortcodes{$f}}{$sig{$f}}) {
		$code = ${$sortcodes{$f}}{$sig{$f}};
	    }
	    print "\t$sig{$f}\t$code";
	} else {
	    print "\t\t";
	}
    }
    my($pqid) = ($i =~ /^.*?:(.*?)\./);
    #    die unless $pqid;
    #    die "no $pqid in tab\n" unless $tab{$pqid};
    if ($pqid) {
	print "\t$i\t$tab{$pqid}";
    } else {
	print "\t0\t";
    }
    print "\n";
}
close(I);

#############################################################################################

sub
expand_insts {
    my @i = split(/\s+/, $_[0]);
    my @new_i = ();
    foreach my $i (@i) {
	if ($i =~ /\+/) {
	    my @iplus = split(/\+/,$i);
	    push @new_i, $iplus[0];
	    my($proj,$rest) = ($iplus[0] =~ /^(.*?):(.*?)$/);
	    push @new_i, map { "$proj:$_" } @iplus[1..$#iplus];
	} else {
	    push @new_i, $i;
	}
    }
    @new_i;
}

sub
set_cgc {
    my @values = @_;
    my %cgc = ();
    my $tmpname = '';
    if (open(TMP,">01tmp/$$.cgc")) {
	$tmpname = "01tmp/$$.cgc";
    } elsif (open(TMP,">/tmp/$$.cgc")) {
	$tmpname = "/tmp/$$.cgc";
    } else {
	die "set_cgc: can't write /tmp/$$.cgc or tmp/$$.cgc\n";
    }
    my $cgctmp = $tmpname;
    print TMP join("_\n", @values), "_\n";
    close TMP;
    system 'msort', '-j', '--out', $tmpname, '-ql', '-n1', 
    '-s', "$ENV{'ORACC'}/lib/config/msort.order", 
    '-x', "$ENV{'ORACC'}/lib/config/msort.exclude", $tmpname;
    open(TMP,$tmpname);
    my @cgc = (<TMP>);
    close(TMP);
    chomp @cgc;
    foreach (@cgc) {
	s/_$//;
    }
    @cgc{@cgc} = (0..$#cgc);
#    print STDERR Dumper \%cgc;
    return \%cgc;
}

1;
