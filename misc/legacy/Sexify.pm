package ORACC::Legacy::Sexify;
use warnings; use strict; use integer;
use utf8;

sub
sexify {
    my ($n,$asz,$utf,$U) = @_;

    return '' unless defined $n;

    return $n if ($n =~ m#^[\d/n]+\(#);

    my $v = ($n =~ s/\@v//);

    my $alef = ($utf ? '₎' : "'");
    my $asz_or_disz = ($asz ? ($utf ? 'aš' : 'asz') 
		       : ($utf ? 'diš' : 'disz'));
    my $shin = ($utf ? 'š' : 'sz');
    my $two = ($utf ? '₂' : '2');

    if ($n =~ m#/#) {
	my $vstr = ($v ? "\@v" : '');
	return "$n(di$shin)$vstr";
    }

    my $szargal = $n / 216000;
    $n %= 216000;

    my $szaru = $n / 36000;
    $n %= 36000;

    my $szar2 = $n / 3600;
    $n %= 3600;

    my $geszu = $n / 600;
    $n %= 600;

    my $geszd = $n / 60;
    $n %= 60;

    my $u = $n / 10;
    $n %= 10;

    my $disz = $n;

    my $ret = '';
    
    $ret .= "$szargal(${shin}ar$two){gal} " if $szargal;
    $ret .= "$szaru(${shin}aru) " if $szaru;
    $ret .= "$szar2(${shin}ar$two) " if $szar2;
    $ret .= "$geszu(ge${shin}u) " if $geszu;
    $ret .= "$geszd(ge$shin$alef$two) " if $geszd;
    $ret .= "$u(u) " if $u;
    $ret .= "$disz($asz_or_disz)" if $disz;

    $ret =~ "\U$ret" if $U;

    $ret .= '@v' if $v;

    $ret =~ s/\s*$//;

    $ret;
}

1;
