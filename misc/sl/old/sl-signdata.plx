#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::SL::BaseC;
use Data::Dumper;

my $arg_project = shift @ARGV;
my $project = $arg_project || 'epsd2';

my @keys = ();
my $sxid = 's00000';
my $vxid = 'v00000';
my $atfline = 1;

my @lines = `xsltproc $ENV{'ORACC_BUILDS'}/lib/scripts/signdata.xsl $ENV{'ORACC_BUILDS'}/$project/01bld/sux/summaries.xml`;
open(L,'>signdata.lines'); print L @lines; close(L);

my %s = ();

foreach (@lines) {
    chomp;
    my ($sn,$vl,$po,$fm,$ey,$id,$cnt,$pct,$cid) = split(/\t/, $_);
    $vl =~ s/-$//;
    push @{${${$s{$sn}}{$vl}}{$po}}, [ $fm, $ey, $id, $cnt, $pct, $cid];
    $sn =~ tr/|()//d;
    $fm =~ tr/|()//d;
    $vl =~ tr/|()//d;
    push @keys, $sn, $vl, $fm;
}

my %sc = set_cgc(@keys);

ORACC::SL::BaseC::init();
$ORACC::SL::report_all = 1;

# print Dumper \%s; exit 0;

open(A, '>01tmp/signdata.asl');
open(ATF,'>01tmp/signdata.atf'); print ATF "&X121212 = signdata.plx forms\n#project: $project\n#atf: use unicode\n#atf: use math\n";
open(X, '|gdlme2 -s>02xml/signdata.xml'); select X;
print '<signlist project="$project" xml:lang="sux" type="glossary-signlist" xmlns="http://oracc.org/ns/sl/1.0" xmlns:g="http://oracc.org/ns/gdl/1.0">';
foreach my $s (sort { &cgccmp; } keys %s) {
    my $sl_sign = ORACC::SL::BaseC::is_sign($s);
    if ($sl_sign) {
	my $sl_name = ORACC::SL::BaseC::sign_of($sl_sign);
	if ($s ne $sl_name) {
	    warn "$0: sign $sl_name is being referred to by list $s\n";
	    next;
	}
    } else {
	warn "$0: so-called sign name $s is not in sign list\n";
	next;
    }
    my $ps = xmlify($s);
    print "<sign n=\"$ps\" xml:id=\"$sxid\">"; ++$sxid;    
    print A "\@sign $s\n";
    my %v = %{$s{$s}};
    foreach my $v (sort { &cgccmp; } keys %v) {
	print A "\@v $v\n" if $v =~ /[aeiu]/;
	my $pv = xmlify($v);
	print "<val n=\"$pv\" xml:id=\"$vxid\">"; ++$vxid;
	my %p = %{$v{$v}};
	foreach my $p (qw/s i m f/) {
	    if ($p{$p}) {
		print "<glo-group type=\"$p\">";
		my @f = @{$p{$p}};
		foreach my $f (sort { &cgccmp2; } @f) {
		    my ($form,$cfgw,$xid,$cnt,$pct,$cid) = @$f;
		    print ATF "$atfline. $form\n"; ++$atfline;
		    my $pform = xmlify($form);
		    my $pcfgw = xmlify($cfgw);
		    print "<glo-inst n=\"$pform\" cfgw=\"$pcfgw\" ref=\"$xid\" cid=\"$cid\" g:me=\"yes\" icount=\"$cnt\" ipct=\"$pct\">$pform</glo-inst>";
		}
		print "</glo-group>";
	    }
	}
	print "</val>";
    }
    print "</sign>";
    print A "\@end sign\n\n";
}
print '</signlist>';
close(X);

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
    warn "cgctmp = $tmpname\n";
    foreach my $t (@values) {
	my $k = $t;
	$t =~ s/^\%.*?://;
	my $t2 = "\L$t";
	if ($t eq '*') {
	    $t = '0';
	} else {
	    $t =~ s/\{.*?\}//g;
	    $t =~ tr/./-/;
	    $k =~ tr/ /_/;
	    $t =~ tr/ /_/;
	    $t2 =~ tr/ /_/;
	}
	print TMP "${t} \t$t2\t$k\n";
	#print TMP join("_\n", @values), "_\n";
    }
    close TMP;
#    system 'msort', '-j', '--out', $tmpname, '-ql', '-n1', 
#    '-s', "$ENV{'ORACC'}/lib/config/msort.order", 
#    '-x', "$ENV{'ORACC'}/lib/config/msort.exclude", $tmpname;
    system 'psdsort', '-i', '-G', '-o', $tmpname, $tmpname;
    open(TMP,$tmpname);
    my @cgc = (<TMP>);
    close(TMP);
    chomp @cgc;
#    foreach (@cgc) {
#	s/_$//;
#    }
#    @cgc{@cgc} = (0..$#cgc);
    @cgc = map { tr/_/ /; m/^.*?\t.*?\t(.*)$/; $1 } @cgc;
    @cgc{@cgc} = (1..($#cgc+1));
#    print STDERR Dumper \%cgc;
    return %cgc;
}
close(ATF);
ORACC::SL::BaseC::term();

############################################################

sub
cgccmp {
    cgccmp_sub($a,$b);
}

sub
cgccmp2 {
    cgccmp_sub($$a[0],$$b[0]);
}

sub
cgccmp_sub {
    my($aa,$bb) = @_;
    $aa =~ tr/|()//d;
    $bb =~ tr/|()//d;
    warn "$aa not in cgc\n" unless $sc{$aa};
    warn "$bb not in cgc\n" unless $sc{$bb};
    return 0 unless $sc{$aa} && $sc{$bb};
    $sc{$aa} <=> $sc{$bb};
}

1;
