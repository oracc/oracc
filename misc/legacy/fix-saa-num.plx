#!/usr/bin/perl
use warnings; use strict; use open ':utf8'; use utf8;
binmode(STDIN, ':utf8'); binmode(STDOUT, ':utf8');

my %complained = ();
my $fname = '';
my %lemtoks = (
    lim=>'līmu[thousand]NU',
    me =>'mē[hundred]NU',
    'me-ma' =>'mē[hundred]NU',
    'me-ni' =>'mē[hundred]NU',
    );

my @lines = ();

foreach my $fn (@ARGV) {
    $fname = $fn;
    open(IN, $fn);
    @lines = (<IN>);
    chomp @lines;
    close(IN);
    fix();
    open(OUT, ">$fn.new");
    print OUT join("\n", @lines), "\n";
    close(OUT);
}

sub
fix {
    for (my $i = 0; $i <= $#lines; ++$i) {
	if ($lines[$i] =~ /^\#lem:/ && ($lines[$i] =~ m/\sn;/ || $lines[$i] =~ m/\sn\s*$/
					|| $lines[$i] =~ /ūm\[day\]/ || $lines[$i] =~ /šattu\[year\]/)) {
	    my $w = $i - 1;
	    $lines[$w] =~ s/\s+(\[?:|[<\(]\$|\%arc)/\cA$1/g;
	    my @words = grep defined && length, split(/\s+/, $lines[$w]);
	    my @lemms = grep defined && length, split(/[:;]\s+/, $lines[$i]);
	    for (my $j = $#lemms; $j; --$j) {

		next unless $words[$j]; # only happens in SAA06_09.atf:5782

		if (($lemms[$j] =~ /^n;?\s*$/ 
		     && $words[$j] !~ /-\[?(?:A|KAM|MESZ|TA|a|e|i|ma|su|szu2|te|ti|tu|x)/
		     && $words[$j] !~ /^[o\/]/
		     && $words[$j] !~ /\(ban2\)/)
		    || ($lemms[$j] =~ /^(ūm\[day\]|šattu\[year\])/ 
			&& $words[$j] =~ /-[0-9]/)) {
		    my @new_w = ();
		    my @new_l = ();
		    $words[$j] =~ s#([0-9]):([0-9])#$1/$2#;
		    warn ":$i:$j: $words[$j]\n" if $words[$j] =~ /:[0-9]/;
		    if ($words[$j] =~ /^[\[<]?(UD|MU)/) {
			$words[$j] =~ s/-/ /;
			push @new_w, split(/\s+/, $words[$j]);
			$lemms[$j] =~ s/\&n//;
			push @new_l, $lemms[$j], 'n';
		    } else {
			my @n = split(/--?/, $words[$j]);
			for (my $n = 0; $n <= $#n; ++$n) {
			    if ($n[$n] =~ /[x0-9]/) {
				push @new_w, $n[$n];
			    } else {
				if ($#new_w >= 0) {
				    if ($new_w[$#new_w] =~ /[x0-9]/
					|| $new_w[$#new_w] eq 'lim') {
					push @new_w, $n[$n];
				    } else {
					$new_w[$#new_w] .= "-$n[$n]";
				    }
				} else {
				    push @new_w, $n[$n];
				}
			    }
			}
			foreach my $w (@new_w) {
			    my ($pre,$word,$meta) = ($w =~ /^([\[\(]*)([-.A-Za-z0-9]+)(.*?)$/);
			    unless ($word) {
				warn ":$fname:$i:$j: failed to split $w into pre/post/meta\n";
				next;
			    }
			    if ($word =~ /^[0-9]/) {
				push @new_l, 'n';
			    } else {
				my $newlem = $lemtoks{$word};
				if ($newlem) {
				    push @new_l, $newlem;
				} else {
				    warn ":$fname:$i:$j: no lemmatization info for $word\n"
					unless $complained{$word}++;
				    push @new_l, 'n';
				}
			    }
			}
		    }
		    splice(@words, $j, 1, @new_w);
		    splice(@lemms, $j, 1, @new_l);
		    print "@new_w\n#@new_l\n";
		}
	    }
	    $lines[$w] = join(' ', @words);
	    $lines[$w] =~ tr/\cA/ /;
	    $lines[$i] = shift(@lemms) . ': ' . join('; ', @lemms);
	}
    }
}

1;
