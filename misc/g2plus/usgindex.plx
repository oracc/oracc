#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
use lib '/usr/local/share/cdl/tools';
use lib '/usr/local/share/epsd/lib';
use CDL::XML;
use CDL::NS;
use CDL::CE;
use CDL::XMD::SortCodes;
use CDL::XMD::SortLabels; CDL::XMD::SortLabels::init();
use CDL::Expand2;
use Getopt::Long;
use Pod::Usage;
use ePSD; ePSD::load();
use PSL; PSL::init();

binmode(STDOUT, 'utf8');
binmode(STDERR, 'utf8');

my $list_file;
my $stdin;
my $verbose;

GetOptions(
	   'list:s'=>\$list_file,
	   'stdin+'=>\$stdin,
	   'verbose+'=>\$verbose,
	   ) or pod2usage(1);

my $xcldoc;

my %data = ();
my %heads = ();
my %fsort = ();
my %pfids = ();
my %psort = ();
my %seen = ();
my %eids = ();
my %eid_sort = ();
my %frm_sort = ();
my %per_sort = ();
my %gen_sort = ();
my %nam_sort = ();
my %uids = ();
my $results = '/usr/local/share/cdl/projects/epsd/cbd/results';

die "usgindex.plx: must give XCL files or use -list or -stdin option\n"
    unless $#ARGV >= 0 || $list_file || $stdin;

if ($#ARGV >= 0) {
    foreach my $a (@ARGV) {
	do_one_file($a);
    }
} elsif ($list_file) {
    open(LST,$list_file) 
	|| die "usgindex.plx: can't read file list $list_file\n";
    while (<LST>) {
	chomp;
	do_one_file($_);
    }
    close LST;
} else {
    while (<>) {
	chomp;
	do_one_file($_);
    }
}
create_sortkeys();
create_index();

#########################################################################

sub
do_one_file {
    my $fn = shift;
 
    if ($fn =~ /^[PQ]\d{6,}[a-z]*$/) {
	$fn = expand("$fn.xcl");
    }

    warn "usgindex.plx: input file $fn non-existent or unreadable\n" 
	and return
	unless -r $fn;

    $xcldoc = load_xml($fn);
    return unless $xcldoc;

    #N.B.: this algorithm only works if each lemma can
    #only participate in a single usage.
    my @usages = tags($xcldoc,$XCL,'usage');
    for (my $i = 0; $i <= $#usages; ++$i) {
	my $u = $usages[$i];
	my $nth = $u->getAttribute('n');
	my $uid = $u->getAttribute('ref');
	my @lsig = ();
	my @fsig = ();
	my $ref = undef;
	if ($nth == 0) {
	    while (1) {
		last if $i > $#usages;
		my $n = $u->parentNode();
		if ($n->localName() eq 'l') {
		    my $lsig = undef;
		    my $fsig = undef;
		    my $cfgw = $n->getAttribute('cfgw');
		    $ref = $n->getAttribute('ref') unless $ref;
		    if ($cfgw) {
			$cfgw =~ s/ʾ/'/g;
			my $eid = ePSD::sid_from_cfgw($cfgw);
			if ($eid) {
			    $eids{$eid} = $cfgw;
			    $lsig = $eid;
			} else {
			    $lsig = 'X'; # $n->getAttribute('cfgw');
			}
			push @fsig, $n->getAttribute('form') || 'X';
		    } elsif ($n->hasAttribute('pos')) {
			$lsig = 'X'; # $n->getAttribute('pos');
			push @fsig, $n->getAttribute('form') || 'X';
		    } else {
			$lsig = 'X';
			push @fsig, 'X';
		    }
		    push @lsig, $lsig;
		} elsif ($n->localName() eq 'll') {
		    ; # TODO: handle ambiguity gracefully
		} else {
		    warn("not handled: ", $n->toString(), "\n");
		}
		last if $u->getAttribute('last');
		$u = $usages[++$i];
	    }
	} else {
#	    die; # can't happen unless data is corrupt
	    ++$i; # happens at present with X u u type usages
	}
	if ($#lsig >= 0) {
	    my $fids = PSL::tlit2ids(join(' ',@fsig));
	    my $f = xmlify(join(" ",@fsig));
	    $pfids{$fids} = $f unless $pfids{$fids};
	    my $lkey = join("\cA",@lsig);
#	    $uids{$lkey} = $uid;
	    ++$uids{$uid};
	    push @{$heads{$lsig[0]}}, $lkey;
	    ++$fsort{$f};
	    push @{${$data{$uid}{'forms'}}{$fids}}, $ref;
	    $ref =~ s/\..*$//;
	    ++$psort{$ref};
	}
    }
}

#use Data::Dumper;
#print Dumper(%data);

sub
create_sortkeys {
    my $i = 0;
    foreach my $e (sort { ecmp($a,$b) } keys %eids) {
	$eid_sort{$e} = $i++;
    }
    open(S,">/tmp/usgindex.srt");
    print S join("\n",keys %fsort), "\n";
    close(S);
    system "psdsort -G /tmp/usgindex.srt >/tmp/usgindex.out";
    open(S,"/tmp/usgindex.out");
    $i = 0;
    foreach my $s (<S>) {
	chomp($s);
	$frm_sort{$s} = $i++;
    }
    close(S);
    foreach my $p (keys %psort) {
	my $xmd = expand("$p.xmd");
	my %codes = CDL::XMD::SortCodes::get($xmd);
	$per_sort{$p} = $codes{'period'};
	$gen_sort{$p} = $codes{'supergenre'};
	$nam_sort{$p} = $codes{'designation'} || $codes{'name'};
    }
}

sub
create_index {
    open(X,">$results/usgindex.xml"); select X;
    print xmldecl(); print '<usgindex>';
#    foreach my $h (sort { hcmp($a,$b) } keys %heads) {
#	print '<sense';
#	if ($h =~ /^e\d+/) {
#	    print " ref=\"$h\"";
#	}
#	print ">";
#	foreach my $l (sort { lcmp($a,$b) } @{$heads{$h}}) {
#	foreach my $l (sort { $uids{$a} cmp $uids{$b} } @{$heads{$h}}) {
	foreach my $uid (sort { $a cmp $b } keys %uids) {
	    next if $seen{$uid}++;
#	    my $uid = $uids{$l};
	    my $puid = $uid;
	    $puid =~ s/^#//;
#	    my $h = key2h($l);
	    print "<usage ref=\"$puid\">";
	    foreach my $f (keys %{$data{$uid}{'forms'}}) {
		print "<form f=\"$pfids{$f}\">";
#		foreach my $r (sort { idcmp($a,$b) } @{${$data{$l}{'forms'}}{$f}}) {
		foreach my $g (genre_groups(@{${$data{$uid}{'forms'}}{$f}})) {
		    print "<div><h3 class=\"usg-genre\">$$g[0]</h3>";
		    foreach my $p (period_groups(@{$$g[1]})) {
			print "<div><h3 class=\"usg-period\">$$p[0]</h3>";
			foreach my $r (sort {&byname($a,$b) } @{$$p[1]}) {
			    my ($label,$line,$lid,$cid) = CDL::CE::line($r,$r);
			    $label =~ s/^\((.*?)\)$/$1/;
			    $label = xmlify($label);
			    print "<ref r=\"$r\" lid=\"$lid\" cid=\"$cid\">$label</ref>";
#		    print "<ref r=\"$r\"/>"
			}
			print "</div>";
		    }
		    print "</div>";
		}
		print "</form>";
	    }
	    print "</usage>";
	}
#	print '</sense>';
#    }
    print '</usgindex>'; close(X);
}

sub
byname {
    my($atmp,$btmp) = @_;
    $atmp =~ s/\..*$//;
    $btmp =~ s/\..*$//;
    $nam_sort{$atmp} <=> $nam_sort{$btmp};
}

sub
genre_groups {
    my @items = @_;
    my %groups = ();
    my $gname = '';
    my @ret = ();
    foreach my $i (@items) {
	$i =~ /^([PQ]\d+)/;
	push @{$groups{$gen_sort{$1}}}, $i;
    }
    foreach my $k (sort { $a <=> $b } keys %groups) {
	push @ret, [ CDL::XMD::SortLabels::get('S',$k), [ @{$groups{$k}} ] ];
    }
    @ret;
}

sub
period_groups {
    my @items = @_;
    my %groups = ();
    my $gname = '';
    my @ret = ();
    foreach my $i (@items) {
	$i =~ /^([PQ]\d+)/;
	push @{$groups{$per_sort{$1}}}, $i;
    }
    foreach my $k (sort { $a <=> $b } keys %groups) {
	push @ret, [ CDL::XMD::SortLabels::get('T',$k), [ @{$groups{$k}} ] ];
    }
    @ret;
}

sub
key2h {
    my $k = shift;
    my @h = ();
    foreach my $h (split(/\cA/, $k)) {
	my $c = ($h =~ /^e\d+/ ? $eids{$h} : undef);
	if ($c) {
	    push @h, $c;
	} else {
	    push @h, $h;
	}
    }
    join(' ', @h);
}

sub
ecmp {
    my($an,$bn) = @_;
    my($as,$bs);
    ($an,$as) = ($an =~ /^e(\d+)\.s(\d+)$/);
    ($bn,$bs) = ($bn =~ /^e(\d+)\.s(\d+)$/);
    $an <=> $bn || $as <=> $bs;
}

sub
lcmp {
    my($a,$b) = @_;
    my @a = split(/\cA/,$a);
    my @b = split(/\cA/,$b);
    for (my $i = 0; $i <= $#a && $i <= $#b; ++$i) {
	my $res = hcmp($a[$i],$b[$i]);
	return $res if $res;
    }
    return $#a - $#b;
}

sub
hcmp {
    my($a,$b) = @_;
    if ($a =~ /^e\d+$/ && $b =~ /^e\d+$/) {
	return ecmp($a,$b);
    } else {
	return $a cmp $b;
    }
}

sub
idcmp {
    my($aq,$bq) = @_;
    $aq =~ s/\..*$//;
    $bq =~ s/\..*$//;
    $per_sort{$aq} <=> $per_sort{$bq};
}

1;
