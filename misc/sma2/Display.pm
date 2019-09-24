package ORACC::SMA2::Display;

use warnings; use strict; use open 'utf8'; use utf8;

use Data::Dumper;
use Encode;

sub
xml_preamble {
    my($id,$n);
    if (defined $_[0]) {
	($id,$n) = ($_[0] =~ /^(.*?)\s*=\s*(.*)$/);
    }
    $id = $id || 'TEST';
    $n = $n || 'TEST';
    print "<text-parses id=\"$id\" n=\"$n\">";
}
sub
xml_postamble {
    print "</text-parses>";
}
sub
xml_parses_o {
    my($w,$o,$i) = @_;
    $w = '#NOWORD#' unless defined $w;
    $o = '' unless defined $o;
    $i = '' unless defined $i;
    print "<parses form=\"$w\" on=\"$o\" wid=\"$i\">";
}
sub
xml_parses_c {
    print "</parses>";
}
sub
xml_parse {
    my $p = shift;
    my %p = %$p;
    my $aroot = ${$p{'root'}}[2];
    my $rr = $aroot;
    $rr = '' unless $rr =~ s/\#.*$//;
    my $dsu = $$p{'dsu'};
    my $dsu_attr = '';

    $dsu_attr = "dsu=\"$dsu\" " if $dsu;

    my $disp = sprint_chains($p);
#    $disp =~ s/^\S+\s+//;

    my($sem,$pos) = ($$p{'pos'} =~ /^(.*?\])(.*)$/);

    if (!defined($sem) || !defined($pos)) {
        $sem = 'X';
        $pos = '';
    }

    $sem =~ tr/_/ /;
    $sem =~ s/\[(?:.*?)=\"(.*?)\"\]/[$1]/;

    my ($prcf,$prgw) = ($sem =~ /^(.*?)(\[.*?\])?$/);
    $sem .= $prgw if $prgw;

    my $semdisp = sem_sprint_chains($sem,$pos,$p);
    my $semonly = $sem;
    if ($pos =~ /^[A-Z]N$/) {
	$semonly = $pos;
    } elsif ($semonly =~ /\[\]$/) {
	$semonly =~ s/\[\]//;
    } else {
	$semonly =~ s/^.*?\[(.*?)\]$/$1/;
	$semonly =~ s/^\(?to(?:\s+be)?\)?\s+// || $semonly =~ s/^(a|the)\s+//;
	$semonly =~ s/^\(?to(?:\s+be)?\)?\s+//;
    }
    $semonly = '~' if $semonly eq '#cvn';

    $sem .= "#$rr" if length $rr;

    $pos = xmlify($pos);
    $sem = xmlify($sem);
    $semonly = xmlify($semonly);
    $semdisp = xmlify($semdisp);

    my $rattr = '';
    if (length ($rr)) {
	$rattr = " redup=\"$rr\"";
    }

    print "<parse ${dsu_attr}pos=\"$pos\" sem=\"$sem\" disp=\"$disp\" semonly=\"$semonly\" semdisp=\"$semdisp\">";
    xcat('vpr', ${$p{'vpr'}}[1]);
    print "<l alias=\"$aroot\"$rattr>", ${$p{'root'}}[1], "</l>";
    xcat('vsf', ${$p{'vsf'}}[1]);
    xcat('nsf', ${$p{'nsf'}}[1]);
    print "</parse>";
}

sub
xmlify {
    my $tmp = shift;
    $tmp =~ s/\&/\&amp;/g;
    $tmp =~ s/\"/\&quot;/g;
    $tmp =~ s/</\&lt;/g;
    $tmp;
}

sub
xcat {
    my ($key,$aref) = @_;
    my @ret = ();
    my @todo = grep(defined&&length, @{$aref});
    if ($#todo >= 0) {
	my $m_index = 0; # slot0 is always empty
	foreach my $m (@{$aref}) {
	    next if $m && ($m eq '0');
#	    $m = '0' unless $m;
	    push @ret, "<m slot=\"$m_index\" n=\"$m\"/>"
		if $m;
	    ++$m_index;
#	    map(s/^(.*)$/<m n=\"$key.$1\"\/>/, @todo);
	}
    }
    if ($#ret >= 0) {
	print "<mm n=\"$key\">";
	print @ret;
	print "</mm>";
    }
}

sub
sprint {
    my $p = shift;
    my %p = %$p;
    my %chains = ('vpr' => mcat(${$p{'vpr'}}[1]),
                  'vsf' => mcat(${$p{'vsf'}}[1]),
                  'isf' => mcat(${$p{'isf'}}[1]),
                  'nsf' => mcat(${$p{'nsf'}}[1]),
                 );
    my $auslaut = auslaut(${$p{'vsf'}}[1])
	|| auslaut(${$p{'nsf'}}[1]);

    my $ret;
#    $ret = (length($p{'pos'}) ? $p{'pos'} : 'O') . ' ';
    $ret = $p{'pos'};
    ($ret =~ s/^(.*?)\[(.*?)\](.*)$/$3\\$1\\$2\\/) || ($ret .= '\\');
#    $ret =~ tr/\[\]/\\\\/;
#    $ret =~ s/\]\\/\\/;
    if (length $chains{'vpr'}) {
	$ret .= $chains{'vpr'};
	$ret .= ':';
    }
    $ret .= ${$p{'root'}}[2]; # use the aliased form for printing
    my $star_a = 0;
    if ($auslaut) {
	$star_a = ($auslaut =~ s/\*$//);
	$ret .= "+$auslaut";
    }
    if (length $chains{'vsf'}) {
	$ret .= ';';
	$ret .= '(a).' if $star_a;
	$ret .= $chains{'vsf'};
    } elsif (length $chains{'isf'}) {
	$ret .= '!';
	$ret .= $chains{'isf'};
    }
    if (length $chains{'nsf'}) {
	$ret .= ',';
	$ret .= $chains{'nsf'};
    }
#    print STDERR "Display: ret=$ret\n";
    $ret;
}

sub
sprint_chains {
    my $p = shift;
    my %p = %$p;
    my %chains = ('vpr' => mcat(${$p{'vpr'}}[1]),
                  'vsf' => mcat(${$p{'vsf'}}[1]),
                  'isf' => mcat(${$p{'isf'}}[1]),
                  'nsf' => mcat(${$p{'nsf'}}[1]),
                 );

    my $ret = '';
#    $ret = (length($p{'pos'}) ? $p{'pos'} : 'O') . ' ';
#    $ret = $p{'pos'};
#    ($ret =~ s/^(.*?)\[(.*?)\](.*)$/$3\\$1\\$2\\/)
#    || ($ret .= '\\');
#    $ret =~ tr/\[\]/\\\\/;
#    $ret =~ s/\]\\/\\/;
    if (length $chains{'vpr'}) {
	$ret .= $chains{'vpr'};
	$ret .= ':';
    }
    my $pr = ${$p{'root'}}[2]; # use the aliased form for printing
    my ($prcf,$prgw) = ($pr =~ /^(.*?)(\[.*?\])?$/);
    $ret .= $prcf;
    $ret .= $prgw if $prgw;
    if (length $chains{'vsf'}) {
	$ret .= ';';
	$ret .= $chains{'vsf'};
    } elsif (length $chains{'isf'}) {
	$ret .= '!';
	$ret .= $chains{'isf'};
    }
    if (length $chains{'nsf'}) {
	$ret .= ',';
	$ret .= $chains{'nsf'};
    }
    $ret;
}

sub
sem_sprint_chains {
    my ($sem,$pos,$p) = @_;
    my %p = %$p;
    my %chains = ('vpr' => mcat(${$p{'vpr'}}[1]),
                  'vsf' => mcat(${$p{'vsf'}}[1]),
                  'isf' => mcat(${$p{'isf'}}[1]),
                  'nsf' => mcat(${$p{'nsf'}}[1]),
                 );

    my $ret = '';
    if (length $chains{'vpr'}) {
	$ret .= $chains{'vpr'};
	$ret .= ':';
    }
#    if ($pos =~ /^[A-Z]N$/) {
#	$sem = $pos;
#    } else {
	$sem =~ s/\[.*?\]//;
#    $sem =~ s/\[\(?to be\)?\s+/[/;
#    $sem =~ s/\[to\s+/[/;
#   }
    
    $ret .= $sem;
    if (length $chains{'vsf'}) {
	$ret .= ';';
	$ret .= $chains{'vsf'};
    } elsif (length $chains{'isf'}) {
	$ret .= '!';
	$ret .= $chains{'isf'};
    }
    if (length $chains{'nsf'}) {
	$ret .= ',';
	$ret .= $chains{'nsf'};
    }
    $ret;
}

sub
mcat {
    my ($aref,$iref,$type) = @_;
    my @m = @{$aref};
    my @i = $iref ? @{$iref} : '';
    shift @m;
    shift @i;
    if ($ORACC::SMA2::extended) {
	my @n = ();
	for (my $i = 0; $i <= $#m; ++$i) {
	    if ($m[$i]) {
		my $slot = sprintf("%02d", $i+1);
		$slot =~ tr/0-9/₀-₉/;
		push @n, "$type$slot=$m[$i]";
	    } else {
		push @n, undef;
	    }
	    if ($i[$i]) {
		push @n, "₀₀$i[$i]";
	    }
	}
	@m = @n;
    }
    my $res = join('.', grep(defined&&length, @m));
    $res =~ s/0(\.|$)//;
    $res =~ s/[,.]$//;
    $res;
}

sub
mcat_x {
    my $aref = shift;
    my $slot_index;
    my @ret;    
    foreach my $slot (@{$aref}) {
	next if $slot && ($slot eq '0');
	push (@ret, '[' . ($slot||'') .']');
    }
    join('',@ret);
}

#FIXME: THIS IS A RIDICULOUS PLACE TO BE EDITING THE AUSLAUT SLOT

sub
auslaut {
    my $aref = shift;
    my @slots = @$aref;
    my $aus = shift @slots;
    if ($aus) {
#	print STDERR "aus=$aus\n";
	# find the first non-empty slot
	@slots = grep $_, @slots;
	if ($slots[0] && $slots[0] =~ /^([aei])/) {
	    # if it starts with a vowel, augment the auslaut
	    my $sv = ($1 eq 'a' ? 'a' : 'e'); # correct for 'i' = LT notation
	    my ($c,$v) = ($aus =~ m/^(.)(.)/);
	    if ($aus =~ s/$v$v/$v/) {
		$aus .= "=$c+$v";
	    } else {
		$aus .= "=$c.$v";
	    }
	} else {
	    # else delete the auslaut
	    $aus = '';
	}
    }
    $aus;
}

sub
ncat {
    my ($aref,$gref) = @_;
    my @m = @{$aref}; shift @m;
    my @g = @{$gref}; shift @g;
    my @res = ();
    for (my $i = 0; $i <= $#m; ++$i) {
	next unless $m[$i] && $m[$i] ne '0';
	if ($g[$i]) {
	    push @res,"$m[$i]<$g[$i]>";
	} else {
	    push @res, $m[$i];
	}
    }
    join('.',@res);
}

sub gcat {
    my ($graph,$igraph,$base,$type) = @_;
    my @g = $graph ? @$graph : (); shift @g;
    my @i = $igraph ? @$igraph : (); shift @i;
    my $s = '';
    for (my $i = 0; $i <= $#g; ++$i) {
	if ($g[$i]) {
	    my $slot = sprintf("%02d", $i+1); $slot =~ tr/0-9/₀-₉/;
	    $s .= "$type$slot=";
	    my @gg = @{$g[$i]};
	    $s .= shift @gg;
	    my $gix = '';
	    if ($#gg > 0) {
		foreach (@gg) {
		    $gix .= 1+$_.'-';
		}
		$gix =~ s/-$//;
	    } else {
		$gix = ''.$gg[0]+1;
	    }
	    1 while $gix =~ s/([0-9])/&upnum($1)/e;
	    $gix =~ tr/-/⁻/;
	    $s .= $gix;
	    $s .= '-';
	}
	if ($i[$i]) {
	    my @ii = @{$i[$i]};
	    $s .= shift @ii;
	    my $gix = '';
	    if ($#ii > 0) {
		foreach (@ii) {
		    $gix .= 1+$_.'-';
		}
		$gix =~ s/-$//;
	    } else {
		$gix = ''.$ii[0]+1;
	    }
	    1 while $gix =~ s/([0-9])/&upnum($1)/e;
	    $gix =~ tr/-/⁻/;
	    $s .= $gix;
	    $s .= '-';
	}
    }
    $s .= base_slot($base);
#    $s =~ s/-$//;
    Encode::_utf8_on($s);
    $s;
}

sub base_slot {
    my $b = shift @_;
    my @b = split(/-/,$b);
    "b₁=".join("₋",@b);
}

sub upnum {
    my %up = (
	0=>'⁰',
	1=>'¹',
	2=>'²',
	3=>'³',
	4=>'⁴',
	5=>'⁵',
	6=>'⁶',
	7=>'⁷',
	8=>'⁸',
	9=>'⁹',
	);
    $up{$_[0]};
}

1;
