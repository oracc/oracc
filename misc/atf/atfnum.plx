#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
use integer;
use Getopt::Long;
binmode STDIN, ':utf8';
binmode STDOUT, ':utf8';

my $raw = 0;
my @todo_list = ();
my $trace = 0;

GetOptions (
    'do:s'=>\@todo_list,
    'raw'=>\$raw,
    'trace'=>\$trace,
    );

my @known_asz = qw/gur gur7 guru7/;
my %known_asz = ();
@known_asz{ @known_asz } = ();

my $numpat = '[^abd-wyzA-Z]+';

#open LOG, ">atfnum.log";
#while (<>) {
#    chomp;
#    if (!/^[&\$\@]/) {
#	handle_line();
#    }
#    print $_, "\n";
#    close if eof ARGV;
#}
#close LOG;

#################################################################

if ($raw) {
    foreach my $n (@ARGV) {
	$_ = "1. $n";
	handle_line();
	s/^\S+\s+//;
	print "$_\n";	
    }
    exit 0;
}

if (@todo_list) {
    $ARGV='TODO';
    $. = 1;
    foreach my $t (@todo_list) {
	$_ = "$.. $t\n";
	handle_line();
	++$.;
	print $_, "\n";
    }
} else {
    while (<>) {
	chomp;
	if (!/^[&\$\@\#<>\|]/) {
	    handle_line();
	}
	print $_, "\n";
	close if eof ARGV;
    }
}

sub
handle_line {
    return unless /[-\s.;\[\+]\d/;
    s/\s*$//;
    s/sze-\./sze 0./g;
#    tr/;/./;
    s/([\s-])\.(?!\.)/${1}0./g;
    s/(?!<[0-9])0\#/0/g;
    my $first_white = '';
    $first_white = $1 if /(\s+)/;
    my ($lnum,$rest) = (/^(\S+\.)\s*(.*)$/);
    my @bits = ();
    if ($lnum) {
	@bits = split(/([-\s\+]+)/,$rest);
	unshift @bits, $lnum;
    } else {
	@bits = split(/([-\s\+]+)/,$_);
    }
    
    for (my $i = 1; $i <= $#bits; ++$i) {

	next if $bits[$i] =~ /^[\s\+]+$/;

	print STDERR "bit: $bits[$i]\n" if $trace;
	
	next if $bits[$i] =~ /[\dnx]\(.*?\)/; # don't reconvert unnecessarily

	$bits[$i] =~ s/(\d)-$/$1 /g;

	$bits[$i] .= '.0' if $bits[$i] =~ /^[\d\+c]+\.[\d\+c]+\??$/;

	$bits[$i] = '0'.$bits[$i] if $bits[$i] =~ /^\.\d/;

	$bits[$i] =~ s/^(\d+)\.(\d+)\.(\d+)\.(\d+)$/$1;$2.$3.$4/g;

	print STDERR "diddled bit: $bits[$i]\n" if $trace;

	if ($bits[$i] =~ /\[?\d/ && $bits[$i] =~ /^\d+[,;]/) {
	    my @steps = split(/[,;.]/,$bits[$i]);
	    my @stepvals = qw/asz barig ban2 disz disz/;
	    my @new = ();
	    for (my $i = 0; $i <= $#steps; ++$i) {
		my $s = $steps[$i];
		next if $s eq '0';
		my $sval = $stepvals[$i];
		if ($sval eq 'asz') {
		    push @new, do_sex_num($s,1);
		} elsif ($sval eq 'barig' || $sval eq 'ban2') {
		    push @new, "$s($sval)";
		} else {
		    push @new, do_sex_num($s);
		}
	    }
	    $bits[$i] = join(' ', @new);
	    next;
	    
	} elsif ($bits[$i] =~ /\d/ 
		 && $bits[$i] =~ /^$numpat[\.;]$numpat\.$numpat/o) {
	    # $bits[$i] =~ tr/././ == 2 && $bits[$i] =~ /-/) {
	    my $start = $i;
	    my $dots = $bits[$i];
	    my $frac1 = '';
	    my $frac2 = '';
	    my $det = '';
	    if ($i < $#bits && $bits[$i+1] =~ /\//) {
		$frac1 = $bits[++$i];
		$bits[$i] = '';
	    }
	    if ($i < $#bits && $bits[$i+1] =~ /\//) {
		$frac2 = $bits[++$i];
		$bits[$i] = '';
	    }
	    if ($i < $#bits && $bits[$i+1] =~ /gana2/i) {
		$det = 'gana2';
	    } else {
		$det = 'gur';
	    }
	    $i = $start;
	    $bits[$i] = do_dot_num($dots,$frac1,$frac2,$det);
	    next;
	}

	next unless $bits[$i] =~ /(^|-)[\(\[<\+]*\d/;
	
	if ($bits[$i] =~ /(i[gt]i.*?-)(\d+)(.*)$/) {
	    my ($pre,$dig,$post) = ($1,$2,$3);
	    $dig = do_sex_num($dig);
	    $bits[$i] = "$pre$dig$post";
	    
	} elsif ($bits[$i] =~ /(^.*?-[\[\(\<]*)([x0-9]+c?)([\*\#\!\?\]\)\>]*.*$)/
	  	  || $bits[$i] =~ /(^.*?[\[\(\<]*)([x0-9]+c?)([\*\#\!\?\]\)\>]*-.*$)/) {
		      my ($pre,$dig,$post) = ($1,$2,$3);
		      $dig = do_sex_num($dig);
		      $bits[$i] = "$pre$dig$post";
		      
	} elsif ($i < $#bits
		 && ($bits[$i+1] =~ /sila3/
		     || ($i+1 < $#bits 
			 && $bits[$i+1] =~ /\// 
			 && $bits[$i+2] =~ /sila3/))) {
	    
#		$bits[$i] =~ s/(\d+)(?!\/)/$1(disz)/;
	    $bits[$i] = do_sex_num($bits[$i]);
	    
	} elsif ($bits[$i] =~ /^[\[\(\<]*\d+[\]\)\>\#*!?]*(?:-|$)/) {
	    $bits[$i] = do_sex_num($bits[$i]);
	    
	} elsif ($i > 0 && check_prev($bits[$i-1])) {
	    $bits[$i] = do_sex_num($bits[$i]);
	    
	} elsif ($i < $#bits) {
	    my $tmp = $bits[$i+1];
	    $tmp =~ tr/\#*!?[]\(\)<>//d;
	    
	    if (exists($known_asz{$tmp})) {
		$bits[$i] = do_sex_num($bits[$i], 1);
	    } else {
		$bits[$i] = do_sex_num($bits[$i]);
	    }
	} else {
	    $bits[$i] = do_sex_num($bits[$i]);
	}
    }
    $_ = join('', @bits[1..$#bits]);
    if (/(^|[-\.\s\+\[\]])[\[\]]*\d+(?![\(\/\d])/) {
	print STDERR "$ARGV:$.: $_\n";
    }
    $_ = $bits[0] . $first_white . $_;
    s/\[\s+/[/g;
    s/\s+\]/]/g;
}

sub
check_prev {
    my $tmp = shift;
    $tmp =~ tr/\#*!?[]()<>//d;
    return $tmp eq 'a-ra2' || $tmp eq 'mu';
}

sub
do_dot_num {
    my ($dots,$frac1,$frac2,$det) = @_;
    if ($det eq 'gana2') {
	return do_dot_area($dots,$frac1,$frac2);
    } else {
	return do_dot_capacity($dots,$frac1);
    }
}

sub
do_dot_area {
    my ($bit,$frac1,$frac2) = @_;
    my $ret = '';
    my ($bur,$esze,$iku) = ($bit =~ /^(.*?)\.(.*?)\.(.*)$/);
    if ($bur !~ /^0(?!\d)/) {
	$ret .= do_bur_num($bur);
	$ret .= ' ';
    }
    if ($esze !~ /^0(?!\d)/) {
	$esze =~ s/(\d+)/$1(esze3)/;
	$ret .= $esze . ' ';
    }
    if ($iku !~ /^0(?!\d)/) {
	$iku =~ s/(\d+)/$1(asz)/;
	$ret .= $iku;
    }
    if (length $frac1) {
	$ret .= ' ' . sex_frac($frac1,1);
    }
    if (length $frac2) {
	$ret .= ' ' . sex_frac($frac2,1);
    }
    $ret;
}

sub
do_dot_capacity {
    my ($bit,$frac) = @_;
    my $ret = '';
    my $c = '';
    $c = 'c' if $bit =~ s/c//g;

    my ($gur,$barig,$ban) = ($bit =~ /^(.*?)\.(.*?)\.(.*)$/);
    if ($gur !~ /^0(?!\d)/) {
	$ret .= do_sex_num($gur.$c,1);
	$ret .= ' ';
    } else {
	$gur =~ s/^0//;
	$ret .= $gur if length $gur;
    }
    if ($barig !~ /^0(?!\d)/) {
	$ret .= do_sex_num($barig.$c);
	$ret =~ s/disz/barig/;
	$ret .= ' ';
    } else {
	$barig =~ s/^0//;
	$ret .= $barig if length $barig;
    }
    if ($ban !~ /^0(?!\d)/) {
	$ban =~ s/(\d+)/$1(ban2)/;
	$ban =~ s/\)/\@c)/ if $c;
	$ret .= $ban;
    } else {
	$ban =~ s/^0//;
	$ret .= $ban if length $ban;
    }
    if (length $frac) {
	$ret .= ' ' . sex_frac($frac.$c);
    }
    $ret =~ s/\s+-/-/g;
    $ret =~ s/\s+/ /g;
    $ret =~ s/\s*$//;
    $ret;
}

sub
do_sex_num {
    my ($bit,$asz) = @_;
    
    return '' unless $bit;

    my ($pre,$dig,$post) = ('','','');

    if ($bit =~ /^\d+(?:\/\d+)?$/) {
	print STDERR "match 1\n" if $trace;
	($pre,$post) = ('','');
	$dig = $bit;
    } elsif ($bit =~ /^(\d+(?:\/\d+)?c?)(.*)$/) {
	print STDERR "match 2\n" if $trace;
	$pre = '';
	$dig = $1;
	$post = $2;
    } elsif ($bit =~ /^\d/) {
	print STDERR "match 3\n" if $trace;
	$pre = '';
	($dig,$post) = ($bit =~ /([\d\/+]*c?)(.*)$/);
    } else {
	print STDERR "match 4\n" if $trace;
	($pre,$dig,$post) = ($bit =~ /^([^0-9]*)([0-9\/+x]+c?)(.*)$/);
    }

    $pre = $pre || '';
    $dig = $dig || '';
    $post = $post || '';

    print STDERR "do_sex_num: $bit => $pre:$dig:$post\n" if $trace;

    return $bit if $post =~ /^x[A-Z]/;

    if (length $dig) {
	my $has_plus = $dig =~ /\+/;
	my $final_plus = $dig =~ /\+$/;
	my @pluses = split(/\+/,$dig);
	$dig = '';
	foreach my $p (@pluses) {
	    my $c = ($p =~ s/c$//);
	    if ($p =~ /\//) {
		$p = sex_frac($p);
	    } else {
		$p = sex_num($p,$asz);
	    }
	    $dig .= $p;
	    $dig =~ s/\)$/\@c\)/ if $c;
	    $dig .= '+' if $has_plus;
	}
	$dig =~ s/\+$// unless $final_plus;
    }
    $dig =~ tr/x/n/;
    "$pre$dig$post";
}

sub
sex_num {
    my ($n,$asz) = @_;

    my $asz_or_disz = ($asz ? 'asz' : 'disz');

    return '' unless defined $n;

    return $n if $n eq 'x';

    return $n if ($n !~ /^\d/);

    my $szargal = $n / 216000;
    $n %= 216000;

    my $szaru = $n / 36000;
    $n %= 36000;

    my $szar2 = $n / 3600;
    $n %= 3600;

    my $geszu = $n / 600;
    $n %= 600;

    my $diszd = $n / 60;
    $n %= 60;

    my $u = $n / 10;
    $n %= 10;

    my $disz = $n;

    my $ret = '';
    
    $ret .= "$szargal(szargal) " if $szargal;
    $ret .= "$szaru(szar'u) " if $szaru;
    $ret .= "$szar2(szar2) " if $szar2;
    $ret .= "$geszu(gesz'u) " if $geszu;
    $ret .= "$diszd(gesz2) " if $diszd;
    $ret .= "$u(u) " if $u;
    $ret .= "$disz($asz_or_disz)" if $disz;

    $ret =~ s/\s*$//;
    
    $ret;
}

sub
sex_frac {
    my ($f,$asz) = @_;
    my $ret = undef;
    if (defined $asz) {
	$ret = "$f(asz)";
    } else {
	$ret = "$f(disz)";
    }
    $ret;
}

sub
do_bur_num {
    my ($bit) = @_;
    my ($pre,$dig,$post) = ($bit =~ /^([^\d]*)([\d\/]*c?)(.*)$/);
    if (length $dig) {
	my $c = ($dig =~ s/c$//);
	$dig = bur_num($dig);
	$dig =~ s/\)$/\@c\)/ if $c;
    }
    "$pre$dig$post";
}

sub
bur_num {
    my ($n) = @_;

    return '' unless defined $n;

    return $n if $n eq 'x';

    my $ret = '';

    my $szar2 = $n / 60;
    $n %= 60;

    my $buru = $n / 10;
    $n %= 10;

    my $bur = $n;

    $ret .= "$szar2(szar2) " if $szar2;
    $ret .= "$buru(bur'u) " if $buru;
    $ret .= "$bur(bur3) " if $bur;

    $ret =~ s/\s*$//;
    
    $ret;
}

1;

__END__

=head1 NAME

atfnum.plx -- fix legacy quantity numbers

=head1 SYNOPSIS

atfnum.plx [file]

=head1 DESCRIPTION

atfnum.plx is a tool for fixing legacy quantity numbers, performing
both syntactic and computational adjustments.  The program makes a
reasonable attempt to convert the various numbering systems
correctly, but B<it is assumed that output will be checked by
humans>.  A good trick is to use the Unix utility 'diff' to show you
which lines have been changed by atfnum.plx

Bare numbers are converted to proper ATF C<REPEATER(UNIT)> syntax, e.g.,
C<1> is converted to C<1(disz)> (or to C<1(asz)> if it is contextually
appropriate).   Decimal numbers are converted to sexagesimal
sequences, e.g., C<35> is converted to C<3(u) 5(disz)>.

=head1 AUTHOR

Steve Tinney (stinney@sas.upenn.edu)

=head1 COPYRIGHT

Copyright(c) Steve Tinney 2004.

Released under the Gnu General Public License
(http://www.gnu.org/copyleft/gpl.html).
