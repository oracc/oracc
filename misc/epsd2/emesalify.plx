#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::XML;
use Encode;
use Data::Dumper;

# add rws="ES" attributes to entry and form nodes which are in emesal but
# not in emegir

# load emegir entries
my %ege = ();
open(EGE,"cbdentries.plx 00src/sux.glo|") || die;
my @ege = (<EGE>); chomp @ege; @ege{@ege} = ();
close(EGE);

# load emegir forms
my %egf = ();
open(EGF, '00src/sux.forms') || die;
my @egf = (<EGF>); chomp @egf; @egf{@egf} = ();
close(EGF);

# load emesal entries, noting which are not in emegir
my %ese = ();
open(ESE, "cbdentries.plx emesal/00lib/sux-x-emesal.glo |") || die;
my @ese = (<ESE>); chomp @ese;
close(ESE);
foreach my $ese (@ese) {
    ++$ese{$ese} unless exists $ege{$ese};
}

# load emesal forms, noting which are not in emegir
my %esf = ();
open(ESF, "epsd2-pull-forms.plx emesal/00lib/sux-x-emesal.glo |");
my @esf = (<ESF>); chomp @esf;
close(ESF);
foreach my $esf (@esf) {
    ++$esf{$esf} unless exists $egf{$esf};
}

open(L,'>01tmp/emesalify.log');
foreach (sort keys %ese) {
    print L "ES word $_\n";
}
foreach (sort keys %esf) {
    print L "ES form $_\n";
}
close(L);

#print Dumper \%esf;

# index the entries and forms to make annotating articles.xml easier
my %es_forms = ();
foreach my $esf (keys %esf) {
    my ($e,$f) = split(/\t/,$esf);
    $f =~ s/^\@form\s+(\S+).*$/$1/;
    $es_forms{$e} = 1;
    $es_forms{"$e\t$f"} = 1;
}

#print Dumper \%es_forms;
my $CBD='http://oracc.org/ns/cbd/1.0';
my $x = load_xml('01bld/sux/sux.g2x');
my @e = tags($x->getDocumentElement(),$CBD,'entry');

#print Dumper \%es_forms;
foreach my $e (@e) {
    my $n = $e->getAttribute('n');
    $n =~ s/\[(.*?)\]/ [$1] /;
#    print $n;
    if ($ese{$n}) {
#	warn "$n is ES\n";
	$e->setAttribute('rws','ES');
    }
    if ($es_forms{$n}) {
	foreach my $f (tags($e,$CBD,'form')) {
	    my $fn = $f->getAttribute('n');
	    my $efn = "$n\t$fn";
	    if ($es_forms{$efn}) {
		$f->setAttribute('rws','ES');
	    }
	}
    }
}

my $str = $x->toString(0);
Encode::_utf8_on($str);
open(X,'>01bld/sux/sux.g2x') || die;
print X $str;
close(X);

1;
