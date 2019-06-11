#!/usr/bin/perl
use warnings; use strict;
use Data::Dumper;

my @order = qw/top compounds bases periods senses base phra equivs bib/;
my %order = (); @order{@order} = 1..9;

foreach my $o (@order) {
    die "$0: ID $o undefined in order; extend values range\n" unless $order{$o};
}

my @abbrev = qw/t c b p s lb lp e b/;
die "$0: bad order/abbrev: @order vs. @abbrev\n" unless $#order == $#abbrev;
my %abbrev = (); @abbrev{@order} = @abbrev;

my @print = qw/Top Compounds Bases Periods Senses Pronunciation Phrases Equivalents Bibliography/;
die "$0: bad order/print: @order vs. @print\n" unless $#order == $#print;
my %print = (); @print{@order} = @print;

my %ox = (); open(OX,'01bld/sux/oidxid.tab'); my @ox = (<OX>); close(OX); chomp @ox;
foreach my $ox (@ox) {
    my($o,$x) = split(/\t/, $ox);
    $ox{$o} = $x;
}

my %t = ();
open(DIV,'01bld/sux/div-ids.tab'); my @div = (<DIV>); close(DIV); chomp @div;
foreach my $div (@div) {
    my($wd,$id) = split(/\t/,$div);
    if ($order{$id}) {
	push @{$t{$wd}}, $id;
    } else {
	warn "$0: no order for $id\n";
    }
}

open(BASE,"$ENV{'ORACC_BUILDS'}/dcclt/02pub/lex-provides-phra-base.txt");
my @base = (<BASE>); close(BASE);
chomp @base;
foreach my $oid (@base) {
    my $xid = $ox{$oid};
    if ($xid) {
	push @{$t{$xid}}, 'base';
    }
}

open(PHRA,"$ENV{'ORACC_BUILDS'}/dcclt/02pub/lex-provides-word-phra.txt");
my @phra = (<PHRA>); close(PHRA);
chomp @phra;
foreach my $oid (@phra) {
    my $xid = $ox{$oid};
    if ($xid) {
	push @{$t{$xid}}, 'phra';
    }
}

my %templates = ();
open(T,'>01bld/sux/templates.tab');
foreach my $t (sort keys %t) {
    my @tsort = sort { $order{$a} <=> $order{$b} } @{$t{$t}};
    my @ttemp = map { $abbrev{$_} } @tsort;
    my $template = join('',@ttemp);
    my $tsort = "@tsort";
    if ($templates{$template}) {
	if ($templates{$template} ne $tsort) {
	    warn "$0: ambiguous template $template == $templates{$template} already defined";
	    warn "$0: attempt to redefine as $tsort not allowed\n";
	}
    } else {
	$templates{$template} = "@tsort";
	html_template($template, @tsort);
    }
    print T "$t\t$template\n";
}

open(T,'>01bld/sux/template-specs.txt');
foreach my $t (sort { length($a) <=> length($b) } keys %templates) {
    print T "$t\t$templates{$t}\n";
}
close(T);

###############################################################################################

sub html_template {
    my($template, @ids) = @_;
    open(H,">01bld/sux/$template.html") || die "$0: can't write to 01bld/sux/$template.html\n";
    print H '<div class="jumps"><ul>';
    foreach my $id (@ids) {
	print H "<li><a href=\"#$id\">$print{$id}</a></li>";
    }
    print H '</ul></div>';
    close(H);
}

1;
