#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
use lib '/usr/local/share/cdl/lib';
use Getopt::Long;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use constant {
    CHR_NONE => 0,
    CHR_BAD =>  1,
    CHR_COMBINE => 2,
    CHR_REPLACE => 3,
    };

use CDL::XML;
use Encode;
my $outfile;

GetOptions(
    'outfile:s'=>\$outfile,
    );

my %badchars = ();
my %badfonts = ();
my %charsets = ();
my $infile = shift @ARGV || '';
die "fixchar.plx: can't read XML input `$infile'; please give a valid file name\n" unless -r $infile;
my $root = load_xml($infile) || die "fixchar.plx: failed to load XML input $infile\n";

iterate($root, '');

if ($outfile) {
    open(X,">$outfile") || die "fixchar.plx: can't write output file `$outfile'\n";
} else {
    open(X,">$infile.new");
}
binmode X, ':raw';
print X $root->toString();
close(X);

1;

#########################################################################

sub
iterate {
    my ($node,$font) = @_;
    my $fontattr = $node->getAttribute('font')
	if $node->isa('XML::LibXML::Element');
    $font = $fontattr if $fontattr;
    foreach my $child ($node->childNodes()) {
	if ($child->isa('XML::LibXML::Element')) {
	    iterate($child,$font);
	} elsif ($child->isa('XML::LibXML::Text')) {
	    my $sub = 0;
	    my $text = $child->getData();
	    Encode::_utf8_on($text);
	    $sub = $node->hasAttribute("sub") if $text =~ /[0-9]/;
	    my $newtext = proc_char($text,$font,$sub);
	    Encode::_utf8_on($newtext);
	    $child->setData($newtext) if $newtext;
	}
    }
}

sub
load_char {
    my $font = shift;
    my $charname = $font;
    $charname =~ tr/ /_/;
    $charname = "/home/cdl/legacy/lib/char/\L$charname.chr";
    if (open(F,$charname)) {
	my %cset = ();
	$cset{'charfile'} = $charname;
	warn "loading font $font from $charname\n";
	my $state = CHR_NONE;
	while (<F>) {
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
		    ${$cset{'replace'}}{$l} = $r;
		} elsif ($state == CHR_COMBINE) {
		    my($l,$r) = lr_split($_);
		    ${$cset{'combine'}}{$l} = $r;
		} else {
		    warn("$charname:$.: can't reach this!\n");
		}
	    } elsif (/^(subdigits|copy|remove|replace|combining|combine)/) {
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
	warn("$infile: no file $charname font $font\n")
	    unless $badfonts{$font}++;
	$charsets{$font} = 0;
    }
    undef;
}

sub
lr_split {
    (/^\s+(\S+?)\s*=>\s*(\S+)/);
}

sub
proc_char {
    my($text,$font,$sub) = @_;
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
unicodify {
    my($s,$c) = @_;
    my @chars = grep(defined&&length, split(/(.)/,$s));
    for (my $i = 0; $i <= $#chars; ++$i) {
	if ($chars[$i] =~ /^$$c{'copy'}$/) {
	    ;
	} elsif ($$c{'remove'} && $chars[$i] =~ /^$$c{'remove'}/) {
	    $chars[$i] = undef;
	} elsif ($chars[$i] =~ /^$$c{'replacing'}$/) {
	    $chars[$i] = $$c{'replace'}{$chars[$i]};
	} elsif ($i && $chars[$i] =~ /^$$c{'combining'}$/) {
	    my $combo = $chars[$i-1].$chars[$i];
	    my $rep = undef;
	    if (($rep = $$c{'combine'}{$combo})) {
		$chars[$i-1] = undef;
		$chars[$i] = $rep;
	    } else {
		print STDERR "$infile: combination `$combo' not found in font $$c{'charfile'}\n";
	    }
	} elsif (ord($chars[$i]) > 127) {
#	    $w = word_of($i,@chars);
	    print STDERR "$infile: char `$chars[$i]' not handled in font $$c{'charfile'}\n"
		unless $badchars{$$c{'charfile'},$chars[$i]}++;
	}
    }
    join('',grep(defined,@chars));
}
