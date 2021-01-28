# Unicode conversion routines for graphemes
#
# Note: the translation table has the unicode
# chars in UTF8.  They will look funny unless
# you are using a Unicode-aware editor with a 
# font that has all the relevant characters.

package ORACC::ATF::Unicode;

use warnings; use strict; use Encode; use utf8;

my %utf8_tab = (
		'c'=>'š',
		'C'=>'Š',
		'sz'=>'š',
		'SZ'=>'Š',
		's,'=>'ṣ',
		'S,'=>'Ṣ',
		't,'=>'ṭ',
		'T,'=>'Ṭ',
		'j' =>'ŋ',
		'J' =>'Ŋ',
		'~g' =>'ŋ',
		'~G' =>'Ŋ',
		's\''=>'ś',
		'S\''=>'Ś',
                '\'s'=>'ś',
		'\'S'=>'Ś',
		'=a'=>'ā',
		'=e'=>'ē',
		'=i'=>'ī',
		'=u'=>'ū',
		'^a'=>'â',
		'^e'=>'ê',
		'^i'=>'î',
		'^u'=>'û',
		'0' =>'₀',
		'1' =>'₁',
		'2' =>'₂',
		'3' =>'₃',
		'4' =>'₄',
		'5' =>'₅',
		'6' =>'₆',
		'7' =>'₇',
		'8' =>'₈',
		'9' =>'₉',
                "`" => 'ʾ',
                "'" => 'ʾ',
		);

my %xascii = (
	      'ḫ'=>'h',
	      'Ḫ'=>'H',
	      '⌈'=>'[#',
	      '⌉'=>'#]',
	      '⸢'=>'[#',
	      '⸣'=>'#]',
	      );

my %acc = (
	   'á'=>['a','2'],
	   'é'=>['e','2'],
	   'í'=>['i','2'],
	   'ú'=>['u','2'],
	   'à'=>['a','3'],
	   'è'=>['e','3'],
	   'ì'=>['i','3'],
	   'ù'=>['u','3'],
	   'Á'=>['A','2'],
	   'É'=>['E','2'],
	   'Í'=>['I','2'],
	   'Ú'=>['U','2'],
	   'À'=>['A','3'],
	   'È'=>['E','3'],
	   'Ì'=>['I','3'],
	   'Ù'=>['U','3'],
	   );

my $utf8_pat = '(';
foreach (sort {length($b)<=>length($a)} keys %utf8_tab) {
    if (/^\d$/) {
	$utf8_pat .= '(?<![-\/])';
    }
    $utf8_pat .= "\Q$_";
    $utf8_pat .= '|';
}
$utf8_pat =~ s/\|$/)/;

my $ascii_pat = '(';
my %ascii_tab = ();
foreach my $u (keys %utf8_tab) {
    next if $u =~ /^[cC]$/;
    $ascii_tab{$utf8_tab{$u}} = $u;
}
foreach my $x (keys %xascii) {
    $ascii_tab{$x} = $xascii{$x};
}
$ascii_tab{'₊'} = 'x';
$ascii_tab{'ₓ'} = 'x';
$ascii_tab{'×'} = 'x';
foreach (sort {length($b)<=>length($a)} keys %ascii_tab) {
    next if /^[cC\`]|s\'$/;
    $ascii_pat .= "$_";
    $ascii_pat .= '|';
}
$ascii_pat =~ s/\|$/)/;

sub
_safeconv {
    my($dig,$rest,$var,$n) 
	= ($_[0] =~ /^((?:\([N\d]+\(|\d*|N(?=\()))(.*?)(~.*)?(\n)?$/);
    $dig = '' unless defined $dig;
    $var = '' unless defined $var;
    $n = '' unless defined $n;
#    print STDERR "rest=$rest\n";
    if ($rest !~ /^(?:.SL|..SL|KWU|LAK|ELLES|ZATU|REC|RSP|BAU|MZL|ABZ|(?:\(\d*)?N\d+)/
	&& $rest ne 'x') {
	$rest =~ s/x(?=[A-Z4])/×/g;
        $rest =~ s/([a-z0-9])x(?!\d)/$1ₓ/g;
	$rest =~ s/$utf8_pat/$utf8_tab{$+}/eog;
    } elsif ($rest eq 'x') {
#	print STDERR "TIMES\n";
	$rest = '×';
    }
    "$dig$rest$var$n";
}

sub
xgconv {
    my $tmp = shift;
#    $tmp =~ s/([a-wyz])x($|-|\()/$1₊$2/g;
    $tmp =~ s/~,/\000/g;
    my $ret = join('', 
		   map(_safeconv($_), 
		       split(/([-.;@\s{}]|x(?![A-Z4+])|(?<!x)\+)/,
			     $tmp)));
    $ret =~ s/\000/~,/g;
    Encode::_utf8_on($ret);
    if ($ret =~ /[×\@]/) {
	$ret =~ s/×₄/×4/g;
	$ret =~ s/\@š/\@c/g;
	$ret =~ s/\@Š/\@C/g;
    }
    $ret;
}

sub
gconv {
    my $tmp = shift;
#    $tmp =~ s/x4(?![\(\d])/\cD/g;
    return $tmp if $tmp =~ /^[xX]$/;
    $tmp =~ s/(~[a-z][a-wyz0-9]*)x/$1\cX/g;
    $tmp =~ s/([a-wyz])x(>![a-zšḫŋṣṭA-ZŠḪŊṢṬ])/$1ₓ/g;
    $tmp =~ s/\cX/x/g;
    my $ret = join('', 
		   map(_safeconv($_), 
		       split(/([-.;+&%\@\s{}x\cD()])/, $tmp)));
    Encode::_utf8_on($ret);
    if ($ret =~ /[×\@]/) {
	$ret =~ s/×₄/×4/g;
	$ret =~ s/\@š/\@c/g;
	$ret =~ s/\@Š/\@C/g;
	$ret =~ s/([-\s(\[])×/$1x/g;
	$ret =~ s/×([-)\]\s\#])/x$1/g;
	$ret =~ s/([<])×([>])/$1x$2/g;
	$ret =~ s/\}×/}x/g;
    }
    $ret;
}

sub
asciify {
    my ($tmp,$a2n) = @_;
    $tmp =~ s/$ascii_pat/$ascii_tab{$+}/eog;
    $tmp = acc2num($tmp) if $a2n;
    $tmp;
}

sub
acc2num {
    my $tmp = shift;
    return $tmp unless $tmp =~ /[áéíúàèìùÁÉÍÚÀÈÌÙ]/;
    my @bits = split(/([-.:&\@%<>(){}\#!?* \t]+)/,$tmp);
    my @newb = ();
    foreach my $b (@bits) {
	$b =~ s/^(.*?)([áéíúàèìùÁÉÍÚÀÈÌÙ])(.*?)$/a2n($1,$2,$3)/eg;
	$b =~ s/\](\d+)$/$1]/;
	push @newb, $b;
    }
    join('',@newb);
}

sub
a2n {
    my($pre,$acc,$post) = @_;
    $pre = '' unless $pre;
    $post = '' unless $post;
    if ($acc{$acc}) {
	my($vow,$num) = (${$acc{$acc}}[0],${$acc{$acc}}[1]);
	"$pre$vow$post$num";
    } else {
	warn("acc2num: accent $acc not in table\n");
	"$pre$acc$post";
    }
}

1;
