#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

#
# -ka-ni
# -bi-da
# -bi
# protect Nanna gissu akkil babbar
# long forms du₁₀ du₁₁ zi(d)--needs manual ensi₂ ša₃ ku₃
# X.X doesn't work
# engify broken after E₂
# engify ga₂ geš lunga? sanga dagal gir₃
# boundary protection for known words ŋeš šag
# 

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

my %eng = (
    
    dingir => 'diŋir',
    gar => 'ŋar',
    geštin => 'ŋeštin',
    'geštu₂' => 'ŋeštug',
    'gir₂' => 'ŋir',
    'giri₃' => 'ŋiri',
    'nig₂' => 'niŋ',
    sag => 'saŋ',
    
    );

foreach my $e (keys %eng) {
    my $E = "\u$e";
    my $V = "\u$eng{$e}";
    $eng{$E} = $V;
}

GetOptions(
    );

# read a list of NN CFs and generate a table of any that change after normalization

while (<>) {
    chomp;
    my $n = normalize($_);
    print "$_\t$n\n" unless $n eq $_;
}

sub normalize {
    my $n = shift;
    $n =~ s/^A\.a\./Aya/
	|| $n =~ s/^Ama\.([aeiu])/Amaʾ$1/
	|| $n =~ s/^E₂\.([aeiu])/Eʾ$1/
	|| $n =~ s/^E₂\.([a-zḫŋšṣṭ])/E$1/ || $n =~ s/^E₂\./E./
	|| $n =~ s/^I₇\.([a-zḫŋšṣṭ])/Id$1/ || $n =~ s/^I₇\./Id./
	;
    $n =~ s/^([AEIU])\.([a-zḫŋšṣṭ])/$1$2/;
    $n = protect_initial_geminate($n,'ab.ba','am.mi','ṣil₂.li₂',);
    $n = engeminate($n,'ṣi.li','ṣi.li₂','i.din');
    $n = engify($n);
    1 while $n =~ s/\.x/.X/;
    1 while $n =~ s/([a-zḫŋṣšṭ])[₀-₉ₓ]+/$1/;
    1 while $n =~ s/([a-zḫŋṣšṭ])\.([a-zḫŋṣšṭ])/$1$2/;
    1 while $n =~ s/([a-zḫŋṣšṭ])\1/$1/;
    $n =~ s/^(A)a/$1/
	|| $n =~ s/^(E)e/$1/
	|| $n =~ s/^(I)i/$1/
	|| $n =~ s/^(U)u/$1/;
    $n =~ tr/\000//d;
    $n;
}

####################################################################################

sub engify {
    my $n = shift;
    my @n = split(/\./,$n);
    my @n2 = ();
    foreach my $nn (@n) {
	push @n2, $eng{$nn} || $nn;
    }
    join('.',@n2);
}

sub engeminate {
    my($n,@g) = @_;
    foreach my $g (@g) {
	my($a,$b) = ($g =~ /^(.*?)\.(.*)$/);
	if ($a) {
	    my $dbl = $b; $dbl =~ s/^(.).*$/$1/;
	    $n =~ s/(^|\.)($a)\.($b)(\.|$)/$1$2$dbl\000$3$4/i;
	} else {
	    die "$0: syntax error in initial_engeminate arg $g\n";
	}
    }
    $n;
}

sub protect_initial_geminate {
    my($n,@g) = @_;
    foreach my $g (@g) {
	my($a,$b) = ($g =~ /^(.*?)\.(.*)$/);
	if ($a) {
	    $n =~ s/^($a)\.($b)/$1\000$2/i;
	} else {
	    die "$0: syntax error in protect_initial_geminate arg $g\n";
	}
    }
    $n;
}

1;
