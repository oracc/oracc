#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

binmode STDOUT, 'utf8';

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::XML;

my %l = ();
open(L,'labels.tab');
while (<L>) {
    chomp;
    my($id,$lab) = split(/\t/,$_);
    $l{$id} = $lab;
    $id =~ s/l$//;
    $l{$id} = $lab;
}
close(L);

my $f = shift @ARGV;
my $x = load_xml($f);

my @forms = tags($x,'http://oracc.org/ns/cbd/1.0','forms');
foreach my $f (@forms) {
    sort_rr($f);
}

sub sort_rr {
    my $f = shift;
    my @rr = tags($f,'http://oracc.org/ns/xis/1.0','rr');
    my @s = ();
    foreach my $rr (@rr) {
#	sort_r($rr);
	push @s, [ $rr->getAttribute('name'), $rr ];
    }
    @s = sort { &rr_cmp } @s;
    remove_nodes($f,@rr);
    foreach my $s (@s) {
	$f->appendChild($$s[1]);
    }
}

sub sort_r {
    my $rr = shift;
    my @r = tags($rr,'http://oracc.org/ns/xis/1.0','r');
    remove_nodes($rr,@r);
    my @s = ();
    foreach my $r (@r) {
	push @s, [ $rr->textContent(), $r ];
    }
    @s = sort { &r_cmp } @s;
    @r = ();
    foreach my $s (@s) {
	$rr->appendChild($$s[1]);
    }
}

sub remove_nodes {
    my($p,@c) = @_;
    foreach my $c (@c) {
	$p->removeChild($c);
    }
}

sub rr_cmp {
    my $aa = $$a[0];
    my $bb = $$b[0];
    $aa =~ s/^.*?_//;
    $bb =~ s/^.*?_//;
    my @aa = split(/\./,$aa); # warn "$aa => '@aa'\n";
    my @bb = split(/\./,$bb); # warn "$bb => '@bb'\n";
    for (my $i = 0; $i <= $#aa; ++$i) {
	my $res = 0;
	if ($aa[$i] =~ /^\d+$/ && $bb[$i] =~ /^\d+$/) {
	    $res = (($aa[$i]||0) <=> ($bb[$i]||0));
	} else {
	    $res = (($aa[$i]||'') cmp ($bb[$i]||''));
	}
	return $res if $res;
    }
    return 0;
}

sub r_cmp {
    my $aa = $$a[0];
    my $bb = $$b[0];
    $aa =~ s/^.*?://;
    $bb =~ s/^.*?://;
    my @aa = split(/\./,$aa);
    my @bb = split(/\./,$bb);
    for (my $i = 0; $i <= $#aa; ++$i) {
	my $res = (($aa[$i]||0) <=> ($bb[$i]||0));
	return $res if $res;
    }
    return 0;
}

$f =~ s/\./+./;
open(X,">$f");
print X $x->toString();
close(X);

1;
