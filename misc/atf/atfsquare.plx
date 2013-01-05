#!/usr/bin/perl -p
use warnings; use strict; use open ':utf8'; 
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $delim = '\.\.\.+|[-.&\@%+ 	{}()<>\|:]';
s/[ \t]*$//;
if (/\[\#/) { # preprocess [# ... #] meaning half-brackets
    my @seq = split(/(\[\#|\#\])/, $_);
    my $hb = 0;
    my @nseq = ();
    foreach my $s (@seq) {
	if ($s eq '[#') {
	    $hb = 1;
	} elsif ($s eq '#]') {
	    $hb = 0;
	} else {
	    if ($hb) {
		$s =~ s/($delim)/\#$1/g;
	        $s .= '#';
	    }
	    push @nseq, $s;
	}
    }
    $_ = join('',@nseq);
}

if (/[\[\]]/) { # only process lines with brackets
    if (tr/[// == tr/]//) { # only process lines with balanced brackets
	chomp;
	my $init_sqb = 0;
        # split into graphemes and simplify
	my @g = split(/($delim)/);
	push @g, "\n";
	my $sqb_open = 0;
	my $needs_initial_sqb = 0;
	my $last_g = 0;
	for (my $i = 0; $i <= $#g; ++$i) {
	    next if $g[$i] !~ /^\.\.\.*/ && ($g[$i] =~ /$delim/ 
					     || $g[$i] =~ /^[\[\]]$/);

	    # skip graphemes that match [ab]
	    next if $g[$i] =~ /^\[[^\[\]]+\][?!*]*$/;

	    $_ = $g[$i];

	    # if this grapheme has an unopened medial closer
	    # append a closer to the previous grapheme unless that one
	    # has a '#' (i.e., don't add closer if it's a[b-c]d)
	    if (/^[^\[\]]+\]./) {
		if ($g[$last_g] !~ /\#/) {
		    $g[$last_g] .= ']';
		    if ($g[$last_g] =~ /$delim/) {
			$g[$last_g] .= ' ';
		    }
		}
	    }

	    # prepend [ if the previous grapheme had an unclosed opener
	    if ($needs_initial_sqb) {
		$needs_initial_sqb = 0;
		$_ = '['.$_;
		$_ = ' '.$_ if /^\[\.\./; # add a space if it was a[b...
	    }

	    # now if we have been working on the pseudo-grapheme '...' we
	    # are done and can skip further diddling
	    if (/\.\.\.+/) {
		$g[$i] = $_;
		$last_g = $i;
		next;
	    }
	    
	    # if this grapheme has an unclosed opener at the end of it
	    # push this onto the front of the next grapheme
	    if (/.\[[^\[\]]*$/) {
		$needs_initial_sqb = 1;
		$last_g = $i;
	    }

	    # remove unlikely [] case so we don't even need to think about it
	    s/\[\]//g;

	    if (/^\[/) {
		# Case 1: initial opener
		if (/.\]./) {
		    # Case 1a: medial closer = [b]a
		    tr/[]//d;
		    $_ .= '#';
		} elsif (/\]$/) {
		    # Case 1b: final closer
		    # do nothing
		} else {
		    # Case 1c: no closer
		    $sqb_open = 1;
		}
	    } elsif (/^[^\[\]]+?\[./) {
		# Case 2: medial opener
		tr/[]//d;
		$_ .= '#';
	    } elsif (/^[^\[\]]+?\[$/) {
		# Case 3: final opener
		$sqb_open = 0;
		tr/[]//d;
	    } elsif (/^[^\[\]]+\]./) {
		# Case 4: no opener, medial closer
		tr/[]//d;
		$_ .= '#';
	    } elsif (/^[^\[\]]+\]$/) {
		# Case 5: no opener, final closer
		# do nothing
	    } else {
		# Case 6: no square brackets in this grapheme
		# do nothing
	    }

	    $g[$i] = $_;
	    $last_g = $i unless $needs_initial_sqb;
	}

	$_ = join('',@g);
	s/\{\[([^\[\]{}]+)\}\]/[{$1}]/g;
	warn("$ARGV:$.: failed to convert internal [ or ]\n")
	    if /[a-z][\[\]][a-z]/i;
	
    } else {
	warn("$ARGV:$.: mismatched [...]\n");
    }
}

s/([\#!?*]+)/cleanup($1)/eg;

sub
cleanup {
    my $tmp = shift;
    if ($tmp =~ tr/\#/\#/ > 1) {
	$tmp =~ tr/\#//d;
	$tmp .= '#';
    }
    $tmp;
}

1;

__END__

=head1 NAME

atfsquare.plx -- fix square brackets

=head1 SYNOPSIS

atfsquare.plx [file]

=head1 DESCRIPTION

atfsquare.plx is a tool for fixing square brackets.  This is useful because in ATF
constructs like e[n] are illegal; you must write 'en#'.  This program inserts the
'#' where necessary and either deletes the brackets or moves them so that they enclose
only graphemes that are completely broken.

If the input contains sequences of the form '[#' ... '#]' these are processed
as half-bracket equivalents and mapped to #-flags in the ATF.

=head1 AUTHOR

Steve Tinney (stinney@sas.upenn.edu)

=head1 COPYRIGHT

Copyright(c) Steve Tinney 2004.

Released under the Gnu General Public License
(http://www.gnu.org/copyleft/gpl.html).
