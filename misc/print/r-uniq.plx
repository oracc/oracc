#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

binmode STDOUT, 'utf8';

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::XML;

my $f = shift @ARGV;
my $x = load_xml($f);

my @forms = tags($x,'http://oracc.org/ns/cbd/1.0','forms');
foreach my $f (@forms) {
    uniq_rr($f);
}

sub uniq_rr {
    my $f = shift;
    my @rr = tags($f,'http://oracc.org/ns/xis/1.0','rr');
    my @s = ();
    foreach my $rr (@rr) {
	uniq($rr);
    }
}

sub uniq {
    my $rr = shift;
    my @r = tags($rr,'http://oracc.org/ns/xis/1.0','r');
    my @new_r = ();
    my $last_label = '';
    for my $r (@r) {
	my $this_label = $r->getAttribute('label2');
	if (!$this_label) {
	    push @new_r, $r;
	} elsif ($last_label && $last_label eq $r->getAttribute('label2')) {
	    # skip this one
	} else {
	    push @new_r, $r;
	    $last_label = $r->getAttribute('label2');
	}
    }
    remove_nodes($rr,@r);
    foreach my $r (@new_r) {
	$rr->appendChild($r);
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
