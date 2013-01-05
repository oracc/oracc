#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
use lib '/usr/local/share/cdl/tools';
use lib '/usr/local/share/epsd/lib';
use CDL::XML;
use CDL::NS;
use CDL::CE;
use CDL::XMD::SortCodes;
use CDL::Expand2;
use Getopt::Long;
use Pod::Usage;
use ePSD; ePSD::load();

binmode(STDOUT, 'utf8');
binmode(STDERR, 'utf8');

my $list_file;
my $stdin;
my $verbose;
my $xclext;

GetOptions(
	   'list:s'=>\$list_file,
	   'stdin+'=>\$stdin,
	   'verbose+'=>\$verbose,
	   'xclext:s'=>\$xclext,
	   ) or pod2usage(1);

my $xcldoc;

my %data = ();
my %heads = ();
my %fsort = ();
my %psort = ();
my %seen = ();
my %eids = ();
my %eid_sort = ();
my %frm_sort = ();
my %per_sort = ();

die "lexindex.plx: must give XCL files or use -list or -stdin option\n"
    unless $#ARGV >= 0 || $list_file || $stdin;

if ($#ARGV >= 0) {
    foreach my $a (@ARGV) {
	do_one_file($a);
    }
} elsif ($list_file) {
    open(LST,$list_file) 
	|| die "lexindex.plx: can't read file list $list_file\n";
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

    warn "lexindex.plx: input file $fn non-existent or unreadable\n" 
	and return
	unless -r $fn;

    $xcldoc = load_xml($fn);
    return unless $xcldoc;

    foreach my $l (tags($xcldoc,$XCL,'d')) {
	next unless $l->getAttribute('type') eq 'line-start';
	#TODO: add support for cell-start/field-start/type=wp;lang=sux
	my $n = $l->nextSibling();
	my @fsig = ();
	my @lsig = ();
	while ($n) {
	    last if $n->localName() eq 'd' 
		&& $n->getAttribute('type') =~ /^(?:line|cell|field)/;
	    if ($n->localName() eq 'l') {
		my $lsig = undef;
		my $fsig = undef;
		my $cfgw = $n->getAttribute('cfgw');
		if ($cfgw) {
		    $cfgw =~ s/ʾ/'/g;
		    my $eid = ePSD::sid_from_cfgw($cfgw);
		    if ($eid) {
			$eids{$eid} = $cfgw;
			$lsig = $eid;
		    } else {
			$lsig = $n->getAttribute('cfgw');
		    }
		    push @fsig, $n->getAttribute('form') || 'X';
		} elsif ($n->hasAttribute('pos')) {
		    $lsig = $n->getAttribute('pos');
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
	    $n = $n->nextSibling();
	}
	if ($#lsig >= 0) {
	    my $f = xmlify(join(" ",@fsig));
	    my $r = $l->getAttribute('ref');
	    my $lkey = join("\cA",@lsig);
	    push @{$heads{$lsig[0]}}, $lkey;
	    ++$fsort{$f};
	    push @{${$data{$lkey}{'forms'}}{$f}}, $r;
	    $r =~ s/\..*$//;
	    ++$psort{$r};
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
    open(S,">/tmp/lexindex.srt");
    print S join("\n",keys %fsort), "\n";
    close(S);
    system "psdsort -G /tmp/lexindex.srt >/tmp/lexindex.out";
    open(S,"/tmp/lexindex.out");
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
    }
}

sub
create_index {
    open(X,">results/lexindex.xml"); select X;
    print xmldecl(); print '<lexindex>';
    foreach my $h (sort { hcmp($a,$b) } keys %heads) {
	print '<lemma';
	if ($h =~ /^e\d+/) {
	    print " ref=\"$h\"";
	}
	print ">";
	foreach my $l (sort { lcmp($a,$b) } @{$heads{$h}}) {
	    next if $seen{$l}++;
	    my $h = key2h($l);
	    print "<usage heading=\"$h\">";
	    foreach my $f (keys %{$data{$l}{'forms'}}) {
		print "<form f=\"$f\">";
		foreach my $r (sort { idcmp($a,$b) } @{${$data{$l}{'forms'}}{$f}}) {
		    my ($label,$line,$lid,$cid) = CDL::CE::line($r,$r);
		    $label =~ s/^\((.*?)\)$/$1/;
		    print "<ref r=\"$r\">$label</ref>";
		}
		print "</form>";
	    }
	    print "</usage>";
	}
	print '</lemma>';
    }
    print '</lexindex>'; close(X);
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
