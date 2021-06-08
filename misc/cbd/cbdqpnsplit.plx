#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use Getopt::Long;
my $pos = undef;
my $qpn = 0;
my $split_lang = undef;
GetOptions(
    'qpn'=>\$qpn,
    'pos:s'=>\$pos,
    'lang:s'=>\$split_lang,
    );

my @g = ();
my @s = ();
use ORACC::CBD::Util;
my $acd_rx = $ORACC::CBD::acd_rx;

$split_lang = 'qpn' if $pos || $qpn;
$pos = 'NN' if $qpn;

die "$0: must give -pos or -lang arg on command line\n" unless $split_lang;

if ($pos) {
    my $pos_match = 0;
    while (<>) {
	if (/\@entry.*\s(\S+)\s*$/) {
	    my $this_pos = $1;
	    if ($qpn) {
		$pos_match = $this_pos =~ /^[A-Z]N$/;
	    } else {
		$pos_match = $pos =~ /$this_pos/;
	    }
	}
	if ($pos_match) {
	    push @s, $_;
	} else {
	    push @g, $_;
	}
    }
} else {
    while (<>) {
	if (/^\@form/) {
	    if (/\%$split_lang /o) {
		push @s, $_;
	    } else {
		push @g, $_;
	    }
	} else {
	    push @g, $_;
	    push @s, $_;
	}
    }
}

open(N,'>split-base.glo');
if ($pos) {
    print N @g;
} else {
    for (my $i = 0; $i < $#g; ++$i) {
	if ($g[$i] =~ /^$acd_rx\@entry/) {
	    my $ok = has_form($i,@g);
	    if ($ok) {
		while ($g[$i]) {
		    if ($g[$i] =~ /^\@bases/) {
			print N $g[$i] unless $split_lang =~ /^sux/;
		    } else {
			print N $g[$i];
		    }
		    last if $g[$i] =~ /^\@end\s+entry/;
		    last if $i > $#g;
		    ++$i;
		}
		print N "\n";
	    }
	}
    }
}
close(N);

if ($pos) {
    open(S,'>split-pos.glo');
    print S @g[0..2];
    print S @s;
    close(S);
} else {
    open(S,'>split-lang.glo');
    for (my $i = 0; $i < $#s; ++$i) {
	if ($s[$i] =~ /^\@proj/) {
	    print S $s[$i];
	    print S "\@lang $split_lang\n";
	    $s[$i] =~ s/\@project\s+//; chomp $s[$i];
	    print S "\@name $s[$i],$split_lang\n\n";
	} elsif ($s[$i] =~ /^$acd_rx\@entry/) {
	    my $ok = has_form($i,@s);
	    if ($ok) {
		while ($s[$i]) {
		    print S $s[$i];
		    last if $s[$i] =~ /^\@end\s+entry/;
		    last if $i > $#s;
		    ++$i;
		}
		print S "\n";
	    }
	}
    }
    close(S);
}

sub has_form {
    my($i,@s) = @_;
    while ($i++ < $#s) {
	if ($s[$i] =~ /^\@form/) { return 1; }
	if ($s[$i] =~ /^\@end/) { return 0; }
    }
    return 0;
}

1;

