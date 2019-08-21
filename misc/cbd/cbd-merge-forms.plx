#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::CBD::Bases;
use ORACC::CBD::Util;
my $acd_rx = $ORACC::CBD::acd_rx;

use Data::Dumper;

my $glo = shift @ARGV;
die "$0: need <LANG>.glo <FORMS>.forms\n" unless $glo;
die "$0: can't read glossary $glo\n" unless -r $glo;
my $forms = shift @ARGV;
unless ($forms) {
    $forms = $glo;
    $forms =~ s/glo$/forms/;
}
die "$0: can't read forms $forms\n" unless -r $forms;

my %bases = ();
my $curr_cfgw = '';
my %f = ();
my @forms = ();
my $printed_forms = 0;
my @projforms = ();
my $project = $glo; $project =~ s/\.glo$//;
my %seen = ();

load_forms($forms, $project);
my $nforms = scalar keys %f; warn "nforms=$nforms\n";
bases_init();
open(G, $glo) || die;
open(G2, ">$glo.m") || die;
select G2;
my $is_compound = 0;
while (<G>) {
    if (/^$acd_rx\@entry\*?\S*\s+(.*?)\s*$/) {
	$curr_cfgw = $1;
	my $cf = $curr_cfgw; $cf =~ s/\s*\[.*//; $is_compound = ($cf =~ /\s/);
	if ($f{$curr_cfgw}) {
	    @forms = map { $$_[1] } @{$f{$curr_cfgw}};
	    delete $f{$curr_cfgw};
	} else {
	    @forms = ();
	}
	$printed_forms = 0;
    } elsif (/^\@bases/ && $bases{$curr_cfgw}) {
	my $incoming = '@bases '.join("; ", sort keys $bases{$curr_cfgw});
	warn "incoming = $incoming\n";
	my $newbref = bases_merge($_, $incoming, $is_compound);
	my $newb = bases_serialize(%$newbref);
	warn "newb = $newb\n";
	$_ = "\@bases $newb\n";
    } elsif (/^$acd_rx\@sense/ && !$printed_forms) {
	if ($#forms >= 0) {
	    print join("\n", uniq(@forms)), "\n";
	    ++$printed_forms;
	}
    }
    print unless /^\@form/; # need to extract forms to 00src/forms, merge, and resplit
}
close(G);
bases_term();
$nforms = scalar keys %f; warn "nforms=$nforms\n";

foreach my $cfgw (sort keys %f) {
    my @proj = map { $$_[0] } @{$f{$cfgw}};
    my %proj = (); @proj{@proj} = ();
    @proj = keys %proj;
    warn "forms for $cfgw were not merged (occurs in @proj)\n";
}

########################################################################

sub
load_forms {
    my($file,$proj) = @_;
    warn "loading $file with project tag $proj\n";
    open(F, $file) || die "cbd-merge-forms.plx: unable to open $file\n";
    while (<F>) {
	chomp;
	my($cfgw,$form) = (/^(.*?)\t(\@form.*)$/);
	$form =~ m/\@form\s+(\S+)/;
	my $orthform = $1;
	if ($form =~ m#\s/(\S+)#) {
	    ++${$bases{$cfgw}}{$1};
	}
	unless ($seen{"$cfgw\:\:$orthform"}++) {
	    push @{$f{$cfgw}}, [ $proj , $form ];
	} else {
	    warn "dropping $form because seen '$cfgw\:$orthform'\n";
	}
	
    }
    close(F);
}

sub uniq {
    my %u = ();
    @u{@_} = ();
    sort keys %u;
}

1;
