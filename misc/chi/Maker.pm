package ORACC::CHI::Maker;
use warnings; use strict; use open 'utf8';
use ORACC::CHI::Checker;
use ORACC::CHI::Fixer;
use ORACC::CHI::Padder;
use ORACC::CHI::Sorter;
use ORACC::CHI::Splitter;

binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

# CHI = Canonical Human-readable Identifier
#
# X_no = Any number that may refer to a text;
#        including museum_no, accession_no,
#         publication_no and possibly others.
#
# X_set = A set of X_no items which form a
#        CHI SET. 
#
# Algorithm for splitting X_set into X_no
# items:
#
# 1) Split on + or & followed by optional
#    ), ?, ! or *, followed by space
#
# Any other massaging should be done before
# passing the input to ORACC::CHI::Maker::set.
#
# Algorithm for making a CHI from an X_no:
#
# 1) Determine prefix:
#    a) if there is a comma-space, split before the 
#       comma (n.b.: comma alone is no good because
#       of 3N-T939,aa numbers
#    b) if there is an alpha-space-number,
#       split at the space
#    c) apply fall-back rules
#    d) if there is a space, split there
#    e) if it is a single token, add chi-ID suffix
#
# 2) Mangle the prefix:
#    a) remove noise characters
#    b) apply prefix-specific rules if any
#    c) otherwise replace delimiters with '-'
#
# 3) Mangle the number
#    a) remove noise characters
#    b) apply prefix-specific rules if any
#    c) otherwise replace delimiters with '-'

my $chi_id = -1;
my $dot_chi = "/Users/stinney/oracc/.chi";
my %prefix_manglers = ();
my %number_manglers = ('BMAcc'=>\&nm_bmacc);

sub
chi {
    my ($id,$in) = @_;
    my ($prefix,$number) = ();
    $in =~ s/^(Ist\s+(?:Ni|O))\.?/unspace($1)/eg;
    $in =~ s/PRAK\s+[12]\s+([ABCD])/PRAK $1/g;
    $in =~ s/PRAK\s+([ABCD])/PRAK-$1/g;
    if ($in =~ /^(.*?),\s+(.*?)/) {
	($prefix,$number) = ($1,$2);
    } elsif ($in =~ /^(.*?[A-Za-z])\s+([0-9].*)$/) {
	($prefix,$number) = ($1,$2);
    } else {
	($prefix,$number) = fallback($in);
	unless ($prefix) {
	    if ($in =~ /^(.*?)\s+(.*?)$/) {
		($prefix,$number) = ($1,$2);
	    } else {
		($prefix,$number) = ($in,chi_id());
	    }
	}
    }
    $prefix = mangle('p', $prefix);
    $number = mangle('n', $prefix, $number);

    ORACC::CHI::Padder::pad($id,$prefix.'_'.$number);
}

sub
chi_id {
    init() if $chi_id == -1;
    sprintf("x%04x", $chi_id++);
}

# This is a place for special purpose splitting of
# numbers.
sub
fallback {
    my $X = shift;
    if ($X =~ /^(\d+)N-T(.*)$/) {
	my $season = $1;
	if (length $season < 2) {
	    $season = "0$season";
	}
	("N${season}T", $2);
    } elsif ($X =~ /^(\d+)N (.*)$/) {
	my $season = $1;
	if (length $season < 2) {
	    $season = "0$season";
	}
	("N${season}", $2);
    } elsif ($X =~ /^(f\')(29.*)$/) {
	("UM-f", $2);
    } elsif ($X =~ /^(FS|L|M|F)-(.*$)$/) {
	($1, $2);
    } else {
	();
    }
}

sub
init {
    $chi_id = load_chi_id();
}

sub
load_chi_id {
    if (-r $dot_chi) {
	my $c = `cat $dot_chi`;
	chomp($c);
	die "ORACC::CHI::Maker: corrupt CHI id in $dot_chi\n"
	    unless $c =~ /^\d+$/;
	0+$c;
    } else {
	0;
    }
}

sub
mangle {
    my($what, $prefix, $number) = @_;
    
    if ($what eq 'p') {
	$prefix =~ tr/ /-/;
	if ($prefix_manglers{$prefix}) {
	    return &{$prefix_manglers{$prefix}}($prefix);
	} elsif ($prefix =~ /^\d+N/) {
	    $prefix =~ s/^(\d+)N$/N$1/;
	    return $prefix;
	} elsif ($prefix eq 'Ni') {
	    $prefix = 'Ist-Ni';
	} else {
	    $prefix = strip_noise($prefix);
	    $prefix = map_delims($prefix);
	    return $prefix;
	}
    } else {
	$number =~ s/^\s*(\S+)\s*$/$1/;
	if ($number eq "\x{2014}") {
	    return chi_id();
	} elsif ($number_manglers{$prefix}) {
	    return &{$number_manglers{$prefix}}($number);
	} else {
	    $number =~ tr/ /-/;
	    $number = strip_noise($number);
	    $number = map_delims($number);
	    return $number;
	}
    }
}

sub
map_delims {
    my $tmp = shift;
    $tmp =~ tr,\,/.;:,-,;
    $tmp;
}

sub
save_chi_id {
    if (open(X, ">$dot_chi")) {
	print X $chi_id;
	close(X);
    } else {
	warn "ORACC::CHI::Maker: unable to save CHI id to $dot_chi\n";
    }
}

# 
# . 	= certain physical join = +
# .- 	= probable physical join = +?
# .. 	= certain indirect join == (+)
# ..- 	= probable indirect join == (+)?
# ...	= multiple discrete objects assigned a single number, 
#	  e.g., tablet and envelope == &
# ...-	= multiple discrete objects probably to be assigned a 
#	  single number (e.g., an envelope in a different 
#	  collection from its table)
#
sub
set {
    my ($id,$s) = @_;
    my $chi = '';
    my @set = ();
    $s =~ tr/\!*//d;
    $s =~ s/\+\?\)/+)?/g;
#    my @splitted = split(/\s+(\(?[+&]\)?\??)\s+/, $s);
    my @splitted = ORACC::CHI::Splitter::split_string($s);
    @splitted = ORACC::CHI::Fixer::fix_joins(@splitted);
    foreach my $X_no (@splitted) {
	$X_no =~ s/^\s*(.*?)\s*$/$1/;
	if ($X_no =~ /^\(?([+&])(\)?)(\??)$/) {
	    my ($j,$p,$q) = ($1,$2,$3);
	    my $x = ($j eq '+' ? '.' : '...');
	    $x .= '.' if $p && $x eq '.'; # ignore parens on &
	    $x .= '-' if $q;
	    push @set, $x;
	} else {
	    if ($X_no =~ /\+/) {
		warn"$id: internal + sign in $X_no\n";
	    } else {
		if ($chi = chi($id,$X_no)) {
		    warn "$id: $X_no produced malformed CHI $chi\n"
			unless ORACC::CHI::Checker::check($chi);
		    push @set, $chi;
		} else {
		    warn "$id: unable to turn $X_no into a CHI\n";
		}
	    }
	}
    }
    ORACC::CHI::Splitter::join_set(
	ORACC::CHI::Sorter::sortuniq_split_set(
	    ORACC::CHI::Splitter::split_set(join('', @set))));
}

sub
strip_noise {
    my $tmp = shift;
    $tmp =~ tr/?!*//d;
    $tmp;
}

sub
term {
    save_chi_id();
}

sub
unspace {
    my $tmp = shift;
    $tmp =~ tr/ /-/;
    $tmp;
}

#######################################################################################
#
# Number manglers
#

sub
nm_bmacc {
    my $n = shift;
    if ($n =~ /^(\d\d\d\d),(\d\d)(\d\d)[,\.]((?:(?:AH|A|Bu)\.?)?[-0-9]+(?:\.[-a-zA-Z]+)*)$/) {
	my $bmacc = "$1-$2-$3";
	my $last = $4;
	$last =~ tr/./-/;
	$last = ((5 - length($last))x'0') . $last;
	"$bmacc-$last";
    } else {
	warn "ORACC::CHI::Maker::nm_bmacc: bad BM accession number format: $n\n";
	$n;
    }
}

sub
nm_uet {
    my $n = shift;
    $n =~ s,6/[123],6,;
    $n;
}

1;
