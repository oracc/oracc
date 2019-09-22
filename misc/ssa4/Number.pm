package ORACC::SSA4::Number;

require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/parse_number has_number atfnum2words/;

use warnings;
use strict;
use utf8;

#
# Cardinals: 1, 2; min, imin
# 
# Ordinals: 1-kam, 1-kam-ma
#
# Fractions: igi-\d-jal2
#

my @word_number = qw/min min3 imin/;
my %word_number = ();
@word_number{ @word_number } = ();

my %number_word = (
    'min'=>'two',
    'imin'=>'seven',
);

my %inwords = ();

my @dishn = qw/1(diš) 2(diš) 3(diš) 4(diš) 5(diš) 6(diš) 7(diš) 8(diš) 9(diš)/;
my @dishw = qw/diš[one] min[two] eš[three] limmu[four] ia[five] aš[six] 
    imin[seven] ussu[eight] ilimmu[nine]/;

my @ashn = qw/1(aš) 2(aš) 3(aš) 4(aš) 5(aš) 6(aš) 7(aš) 8(aš) 9(aš)/;
my @ashw = qw/aš[one] min[two] eš[three] limmu[four] ia[five] aš[six] 
    imin[seven] ussu[eight] ilimmu[nine]/;

my @un = qw/1(u) 2(u) 3(u) 4(u) 5(u)/;
my @uw = qw/u[ten] niš[twenty] ušu[thirty] nimin[fourty] ninnu[fifty]/;

my @geshn = qw/1(ŋeš) 2(ŋeš) 3(ŋeš) 4(ŋeš) 5(ŋeš) 6(ŋeš) 7(ŋeš) 8(ŋeš) 9(ŋeš)/;
my @geshw = qw/ŋeš[sixty] ŋešmin[two-sixties] ŋešeš[three-sixties]
    ŋešlimmu[four-sixties] ŋešia[five-sixties] ŋešaš[six-sixties]
    ŋešimin[seven-sixties] ŋešussu[eight-sixties] ŋešilimmu[nine-sixties]/;

my @sgaln = qw/1(šargal)/;
my @sgalw = qw/šargaldiš/;

@inwords{@dishn} = @dishw;
@inwords{@ashn} = @ashw;
@inwords{@un} = @uw;
@inwords{@geshn} = @geshw;
@inwords{@sgaln} = @sgalw;

$inwords{'1/2(diš)'} = 'sa[one-half]';

sub
atfnum2words {
    my $n = shift;
    $n =~ s/\[.*$//;
    $n = lc($n);
    $n =~ tr/cj/šŋ/;
    my $ret = $inwords{$n};
    print STDERR "atfnum2words: no entry for '$n'\n"
	unless $ret;
    $ret;
}

sub
has_number {
    my @g = @_;
    return 1 if exists $word_number{$g[0]};
#    return 1 if $g[0] eq 'igi' && defined($g[1]) && $g[1] =~ /^\d/;
    return 1 if $g[0] =~ /^\d/;
#    foreach my $g (@g) {
#	i
#	return 1 if ($g =~ /^\d/); # || (exists $word_number{$g}); # can't do this--breaks a-da-min3 etc
#    }
    0;
}

sub
parse_number {
    my @g = @_;
#    if ($g[0] =~ /^igi$/) {
#	return ('igijal',"1/$g[1]",join('-',@g[0..2]),'Qf', 3);
#    } els
    if ($g[0] =~ /^\d/ || exists($word_number{$g[0]})) {
	if ($#g > 0 && $g[1] =~ /^kam$/) {
	    my $nsf_start = 2;
	    ++$nsf_start if (defined($g[2]) && $g[2] =~ /^ma$/);
	    my $cf = "$g[0].kamak";
	    return($cf,
		   ordinal($number_word{$g[0]}||$g[0]),
		   $number_word{$g[0]}||$g[0],'Qo',$nsf_start);
	} else {
	    return($g[0],$number_word{$g[0]}||$g[0],$g[0],'Qc',1);
	}
    } else {
	return (undef,undef,undef,undef,-1);
    }
}

sub
ordinal {
    my $card = shift;
    my %o = ('one'=>'first','two'=>'second','three'=>'third',);
    $card . ($o{$card} || 'th');
}

1;
