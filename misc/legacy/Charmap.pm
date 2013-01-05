package ORACC::Legacy::Charmap;
use warnings; use strict; use open 'utf8'; use utf8;
use lib "$ENV{ORACC}/lib";

use constant {
    CHR_NONE => 0,
    CHR_BAD =>  1,
    CHR_COMBINE => 2,
    CHR_REPLACE => 3,
    };

my %badchars = ();
my %badfonts = ();
my %charsets = ();
my $trace = 0;
my $odf = undef;

1;

#########################################################################

sub
charof {
    my $c = shift;
    if ($c =~ s/^\"(.*?)\"$/$1/) {
	return $c;
    } elsif ($c =~ /^0x/) {
	return chr($c);
    } else {
	return $c;
    }

}

sub
load_char {
    my $font = shift;
    my $charname = $font;
    $charname =~ tr/ /_/;
    $charname = "$ENV{'ORACC'}/lib/charmap/\L$charname.chr";
    if (open(F,$charname)) {
	my %cset = ();
	$cset{'charfile'} = $charname;
	warn "Charmap: loading font $font from $charname\n" if $trace;
	my $state = CHR_NONE;
	while (<F>) {
	    chomp;
	    if (/^\#/ || /^\s*$/) {
		next;
	    } elsif (/^\s/) {
		if ($state == CHR_BAD) {
		    next;
		} elsif ($state == CHR_NONE) {
		    warn("$charname:$.: nowhere to put char spec\n");
		    next;
		} elsif ($state == CHR_REPLACE) {
		    my($l,$r) = lr_split($_);
		    ${$cset{'replace'}}{charof($l)} = charof($r);
		} elsif ($state == CHR_COMBINE) {
		    my($l,$r) = lr_split($_);
		    $r =~ s/^\"(.*?)\"$/$1/;
		    ${$cset{'combine'}}{$l} = $r;
		} else {
		    warn("$charname:$.: can't reach this!\n");
		}
	    } elsif (/^(subdigits|copy|remove|replace|combining|combinations)/) {
		if (/^subdigits\s*=\s*(\d+)\s*$/) {
		    $cset{'sub'} = $1;
		    $state = CHR_NONE;
		} elsif (/^copy\s*=\s*(\S*)/) {
		    $cset{'copy'} = "\[$1\]";
		    $state = CHR_NONE;
		} elsif (/^remove\s*=\s*(\S*)\s*$/) {
		    my $x = $1;
		    if (length $x) {
			$cset{'remove'} = "\[$x\]";
		    }
		    $state = CHR_NONE;
		} elsif (/^combining\s*=\s*(\S+)/) {
		    $cset{'combining'} = "\[$1\]";
		    $state = CHR_NONE;
		} elsif (/^replace/) {
		    $state = CHR_REPLACE;
		} elsif (/^combine/) {
		    $state = CHR_COMBINE;
		}
	    } else {
		warn("$charname:$.: syntax error\n");
		$state = CHR_BAD;
	    }
	}
	if ($cset{'replace'}) {
	    $cset{'replacing'} = join('','[',keys %{$cset{'replace'}},']');
	} else {
	    $cset{'replacing'} = '';
	}
	$cset{'combining'} = '' unless $cset{'combining'};
	$cset{'copy'} = '' unless $cset{'copy'};
#	use Data::Dumper;
#	print STDERR Dumper(\%cset);
	return $charsets{$font} = { %cset };
    } else {
	warn("$odf: no file $charname font $font\n")
	    unless $badfonts{$font}++;
	$charsets{$font} = 0;
    }
    undef;
}

sub
lr_split {
    (/^\s+(\S+?)\s*=>\s*(\".*?\"|\S+)/);
}

sub
map_char {
    my($text,$font,$sub) = @_;
    return $text unless $font;
    my $char = $charsets{$font} || load_char($font);
    return unless $char;
    if ($sub) {
	$text =~ tr/0-9x/₀-₉ₓ/;
	$text;
    } else {
	unicodify($text, $char);
    }
}

sub
set_odf {
    $odf = $_[0];
}

sub
set_trace {
    $trace = $_[0];
}

sub
unicodify {
    my($s,$c) = @_;
    my @chars = grep(defined&&length, split(/(.)/,$s));
    for (my $i = 0; $i <= $#chars; ++$i) {
	if ($$c{'copy'} && $chars[$i] =~ /^$$c{'copy'}$/) {
	    warn "Charmap: copied `$chars[$i]'\n" if $trace;
	} elsif ($$c{'remove'} && $chars[$i] =~ /^$$c{'remove'}/) {
	    $chars[$i] = undef;
	    warn "Charmap: removed `$chars[$i]'\n" if $trace;
	} elsif ($$c{'replacing'} && $chars[$i] =~ /^$$c{'replacing'}$/) {
	    my $orig = $chars[$i];
	    $chars[$i] = $$c{'replace'}{$chars[$i]};
	    warn "Charmap: replaced `$orig' with `$chars[$i]'\n" if $trace;
	} elsif ($$c{'combining'} && $i && $chars[$i] =~ /^$$c{'combining'}$/) {
	    my $combo = $chars[$i-1].$chars[$i];
	    my $rep = undef;
	    if (($rep = $$c{'combine'}{$combo})) {
		my $orig = "$chars[$i-1]\+$chars[$i]";
		$chars[$i-1] = undef;
		$chars[$i] = $rep;
		warn "Charmap: replaced combo `$orig' with `$chars[$i]'\n" if $trace;
	    } else {
		print STDERR "$odf: combination `$combo' not found in font $$c{'charfile'}\n";
	    }
	} elsif (ord($chars[$i]) > 127) {
#	    $w = word_of($i,@chars);
	    print STDERR "$odf: char `$chars[$i]' not handled in font $$c{'charfile'}\n"
		unless $badchars{$$c{'charfile'},$chars[$i]}++;
	}
    }
    join('',grep(defined,@chars));
}
