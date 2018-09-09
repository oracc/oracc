#!/usr/bin/perl
use warnings; use strict; use utf8; use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::SL::Alias;
binmode STDOUT, 'utf8';
my %terms = ();
my %aterms = ();
my %complex = ();
my %primes = ();
my %nonprimes = ();
my %skeys = ();
my %nepsd = ();
my %ix = ();
my $skid = 'aaaa';
my $nid = '0000001';

ORACC::SL::Alias::init();
while (<>) {
    s/\%sux://g;
    my($term,$year,$auth,$publ,$page) 
	= (/^(.*?)\s+::\s+(.*?)\s+::\s+(.*?)\s+::\s+(.*?)\s+::\s+(.*?)\s*$/);
    my $simple = "$auth $publ $page";
    $simple .= " [$year]" if $year;
    $simple =~ s/^\s*//; $simple =~ s/\s*$//;
    $term =~ s/--+/ /g;
    $term =~ s/\.\.\./ /g;
    $term =~ s/\s+/ /g;
    $term =~ s/^\s*//; $term =~ s/\s*$//;
    $term = ORACC::SL::Alias::alias_words($term);
    unless (${$aterms{$term}}{$simple}++) {
	if ($publ eq 'ePSD') {
	    ++$nepsd{$term};
	}
	warn("$ARGV:$.: no data\n") unless $publ || $page;
	$complex{$simple} = [$year,$auth,$publ,$page];
	push @{$terms{$term}}, $simple;
	++$primes{$simple} if $ARGV eq '00src/epsd+prime.txt';
    }
    close ARGV if eof ARGV;
}

#########################################################################################

open(SORT,">01tmp/issl.in");
foreach my $t (keys %terms) {
    my $k = make_skey($t);
    $skeys{$skid} = $t;
    print SORT "$k\t$skid\n";
    ++$skid;
}
close(SORT);
system "psdsort -G -t\\t 01tmp/issl.in >01tmp/issl.out";

open(XML,">01bld/issl.xml");
print XML '<?xml version="1.0" encoding="utf-8"?>',"\n";
print XML "<issl>";
open(IN,"01tmp/issl.out");
while (<IN>) {
    my($keys,$id) = (/^(.*?)\t(.*?)$/);
    my $pos = 0;
    foreach my $k (split(/\s/,$keys)) {
	$k =~ tr/[A-ZŠŊ]/[a-zšŋ]/;
	push @{$ix{$k}}, "$nid/$pos";
    }
    my $term = $skeys{$id};
    my $xterm = xmlify($term);
    print XML "<entry xml:id=\"sux.x$nid\">";
    print XML "<term>$xterm</term><bib>";
    foreach my $p (@{$terms{$term}}) {
	if ($p =~ s/^ePSD\s+//) {
	    my $eid = $p;
	    $p =~ s/^\S+\s*//;
	    $p = xmlify($p);
	    my $cfgw = $p;
	    $p =  '<span class="cf">'.$p;
	    $p =~ s/\[(.*?)\]/<\/span><span class="gw"> [\U$1]\L<\/span>/;
	    $eid =~ s/\s.*$//;
	    my $class = ($eid =~ s/^\*// ? 'epsdprime' : 'epsdissl');
	    print XML "<ref><a href=\"javascript:showarticle('$eid.html')\" class=\"$class\">",
	    $p,"</a></ref>";
	    unless ($class =~ /r/) { # only process non-primes
		foreach my $simple (@{$terms{$term}}) {
		    next if $simple =~ /^<span/; # skip ePSD entries
		    next if $primes{$simple};
		    next if $simple =~ /ePSD|AHw/;
		    ++${$nonprimes{"$eid/$cfgw"}}{$simple}
		      unless $nepsd{$term} > 1;
		}
	    }
	} else {
	    print XML "<ref>",xmlify($p),"</ref>";
	}
    }
    print XML "</bib>";
    print XML "</entry>";
    ++$nid;
}
close(IN);
print XML "</issl>";
close(XML);

open(NONP,">01bld/nonprimed.xml");
print NONP '<nonprimes>';
foreach my $k (sort { &eidcmp } keys %nonprimes) {
    my ($eid,$cfgw) = ($k =~ /^(.*?)\/(.*)$/);
    $cfgw = asciify($cfgw);
    print NONP "<entry ref=\"$eid\" cfgw=\"$cfgw\">";
    foreach my $r (sort { &refcmp } keys %{$nonprimes{$k}}) {
	my @fields = qw/year auth publ loc/;
	my $rref = $complex{$r};
	die "lost ref to $r" unless $rref;
	print NONP '<ref>';
	for (my $i = 0; $i < 4; ++$i) {
	    $$rref[$i] = asciify($$rref[$i]||'');
	    print NONP "<$fields[$i]>",xmlify($$rref[$i]||''),"</$fields[$i]>";
	}
	print NONP '</ref>';
    }
    print NONP '</entry>';
}
print NONP '</nonprimes>';
close(NONP);

sub
eidcmp {
    my ($ad,$bd) = (0,0);
    $a =~ /(\d+)/; $ad = $1;
    $b =~ /(\d+)/; $bd = $1;
    $ad <=> $bd;
}

sub
refcmp {
    get_yr(${$complex{$b}}[0]) <=> get_yr(${$complex{$a}}[0]);
}

sub
get_yr {
    my $tmp = shift;
    return $1 if $tmp =~ /(\d\d\d\d)$/;
    return $1 if $tmp =~ /^(\d\d\d\d)$/;
    return 0;
}

sub
make_skey {
    my $k = shift;
    $k =~ tr/.%+:\//----/;
    $k =~ s/×/-/g;
    $k =~ s/\(.*?\)//g;
    $k =~ s/\{.*?\}//g;
    $k =~ s/\s+/ /g;
    $k;
}

sub
xmlify {
    my $x = shift;
    $x =~ s/\&/\&amp;/g;
    $x =~ s/</\&lt;/g;
    $x;
}

sub
asciify {
    my $tmp = shift;
    $tmp =~ tr/šŠŋŊöé₀-₉/cCjJoe0-9/;
    $tmp;
}

1;
