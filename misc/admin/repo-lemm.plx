#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDERR, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;

use Data::Dumper;

my $BOM = "\xff\xfe";

my @fields = qw/form lang cf gw sense pos epos norm base cont morph1 morph1/;
my @insts = ();
my %keyvals = ();
my @pool = ();
my %sortcodes = ();
my $state = 0; # 0 = pre-stringpool; 1 = stringpool
my %tab = ();
my @texts = ();

my $docorder = '00000001';

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
#	$tab .= "\t$pool[$str]\t$sort";
	$tab .= "\t$pool[$str]";
    }
    $tab{$pqid} = $tab;
}

open(P, '01bld/project.sig');
while (<P>) {
    chomp;
    my($sig,$count,$insts) = split(/\t/);
    $sig =~ s/\&\&.*$//; ### FIXME: must handle COFs and PSUs properly
    my %sig = parse_sig($sig);
    $sig{'form'} =~ s/\%.*?://g;
    $sig{'form'} = downcase_semdet($sig{'form'});
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

foreach my $f (@fields) {
    my @vals = keys %{$keyvals{$f}};
    next unless $#vals > 0;
    my $cgc_ref = set_cgc(@vals);
    my %cgc = %$cgc_ref;
    foreach my $v (@vals) {
	${$sortcodes{$f}}{$v} = $cgc{$v};
    }
}

my %labeltable = ();
open(L,"cat 01bld/lists/have-xtf.lst | labeltable |");
while (<L>) {
    chomp;
    my($id,$lab) = split(/\t/,$_);
    $labeltable{$id} = $lab;
}
close(L);

my %unitinfo = ();
open(L,"cat 01bld/lists/have-xtf.lst | unittable |");
while (<L>) {
    chomp;
    my($id,$unitinfo) = (/^(.*?)\t(.*?)$/);
    $unitinfo{$id} = $unitinfo;
}
close(L);

use Data::Dumper; open(K,'>unitinfo.dump'); print K Dumper \%unitinfo; close(K);
#exit 1;

@insts = sort { &instcmp } @insts;

#use Data::Dumper; open(K,'>insts.dump'); print K Dumper \@insts; close(K);

open(I,'>01bld/repo-lemm.tab'); select I;
#print $BOM;
print "project\tword_id\ttext\tline_label\tFORM\tLANG\tCF\tGW\tSENSE\tPOS\tEPOS\tNORM\tBASE\tCONT\tMORPH1\tMORPH2\tdiscourse_id\tdiscourse_type\tsentence_id\tsentence_label\tgenre\tplace\tperiod\n";
foreach my $inst (@insts) {
    my($i,$sigref) = @$inst;
    my %sig = %$sigref;
    my ($xp,$xi) = ($i =~ m/^(.*?):(.*?)$/);
    my $line = $xi;
    $line =~ s/.\d+$//;

    my($pqid) = ($i =~ /^.*?:(.*?)\./);
    die unless $pqid;
    die "no $pqid in tab\n" unless $tab{$pqid};
    my $meta = $tab{$pqid};
    my ($PQid,$des,$rest) = ($meta =~ /^(.*?)\t(.*?)\t(.*?)$/);

#    print "$docorder\t$xp\t$xi\t$des\t$labeltable{$line}\t"; ++$docorder;
    print "$xp\t$xi\t$des\t$labeltable{$line}\t";
    
    print "$sig{'form'}";
    foreach my $f (@fields[1..$#fields]) { # skip form
	if ($sig{$f}) {
	    my $code = 0;
	    if ($sortcodes{$f} && ${$sortcodes{$f}}{$sig{$f}}) {
		$code = ${$sortcodes{$f}}{$sig{$f}};
	    }
#	    print "\t$sig{$f}\t$code";
	    print "\t$sig{$f}";
	} else {
#	    print "\t\t";
	    print "\t";
	}
    }
    print "\t$unitinfo{$xi}";
    print "\t$rest";
    print "\n";
}
close(I);

#############################################################################################

sub
downcase_semdet {
    my $tmp = shift;
    1 while $tmp =~ s/\{(IM|MUŠEN|KI|URU|GIŠ|ID₂|GU₄|KUR|ANŠE|LU₂|MUNUS|SIG₂|NA₄|TUG₂|UDU)\}/\L{$1}/g;
    $tmp;
}

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
instcmp {
    my($aproj,$aid,$aline,$aword) = ($$a[0] =~ /^(.*?):(.*?)\.(.*?)\.(.*?)$/);
    my($bproj,$bid,$bline,$bword) = ($$b[0] =~ /^(.*?):(.*?)\.(.*?)\.(.*?)$/);
    my $tmp = $aproj cmp $bproj;
    return $tmp if $tmp;
    $tmp = $aid cmp $bid;
    return $tmp if $tmp;
    $tmp = $aline <=> $bline;
    return $tmp if $tmp;
    $aword <=> $bword;
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
