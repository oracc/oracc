#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";
use Data::Dumper;

my $lang = shift @ARGV;

die "$0: must give language on command line. Stop.\n" unless $lang;

my $glo = `oraccopt . $lang`;  $glo = "." unless $lang;
my $proj = `oraccopt`;

my %f = ();
my %ranks = ();
my $r = -1;

my $union = "01bld/$lang/union.sig";
die "$0: nothing to do because no $union. Stop.\n"
    unless -r $union;

foreach my $p (split(/\s+/,$glo)) {
    my $gloname = set_gloname($p);
  file:
    {
	if (open(G,$gloname)) {
	    while (<G>) {
		next if /^\@(?:proj|name|lang)/;
		if (/^\@fields/) {
		    set_fields($_);
		    if (defined $f{'rank'}) {
			$r = $f{'rank'};
		    } else {
			warn "$0: no rank in sigs, ignoring $gloname\n";
			close(G);
			next file;
		    }
		} else {
		    my @sigs = (<G>); chomp @sigs;
		    close(G);
		    foreach my $s (@sigs) {
			next unless $s =~ /\%$lang/;
			my @s = split(/\t/, $s);
			my $sig = $s[0]; $sig =~ s/^.*?://;
			$ranks{$sig} = $s[$r]
			    unless defined $ranks{$sig};
		    }
		    next file;
		}
	    }
	} else {
	    warn "$0: unable to open glossary $gloname as referenced in $lang\n";
	}
    }
}

# open(R,'>R'); print R Dumper \%ranks; close(R);

open(U,"cut -f1 < $union|") || die "$0: $union no longer readable. Surprising. Stop.\n";
open(R,">01bld/$lang/union-ranks.sig") || die "$0: unable to write 01bld/$lang/union-ranks.sig\n"; select R;
while (<U>) {
    next if /^\s*$/;
    if (/^\@(?:proj|lang|name)/) {
	print;
    } elsif (/^\@field/) {
	print "\n\@fields sig rank\n";
    } else {
	chomp;
	my $s = $_; $s =~ s/^.*?://;
	if (defined $ranks{$s}) {
	    print "$_\t$ranks{$s}\n";
	} else {
	    print "$_\t0\n";
	}
    }
    
}
close(R);
close(U);

##################################################################################

sub set_fields {
    my $fields = shift;
    %f = ();
    my @f = split(/\s/, $fields);
    shift @f; # drop '@field';
    for (my $i = 0; $i <= $#f; ++$i) {
	$f{$f[$i]} = $i;
    }
}

sub set_gloname {
    my $p = shift;
    if ($p eq '.') {
	return "01bld/$lang/from_glo.sig";
    } else {
	# TODO: handle qualified proj/lang combos
	if ($p =~ /names/) {
	    return "$ENV{'ORACC_BUILDS'}/$p/01bld/qpn/from_glo.sig";
	} else {
	    return "$ENV{'ORACC_BUILDS'}/$p/01bld/$lang/from_glo.sig";
	}
    }
}

1;
