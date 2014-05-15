#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use Data::Dumper;

my @input = ();
my $verbose = 1;

read_input();
dump_phase_1() if $verbose;

########################################################################################################

sub
dump_phase_1 {
    open(D, ">unpack1.dump");
    print D Dumper \@input;
}

sub
read_input {
    my $current_composite_link = ();
    my @current_protocols = ();
    my $input = shift @ARGV;
    my %sigla = ();

    open(IN, $input) || die "xsf-unpack-P.plx: unable to read '$input'\n";
    while (<IN>) {
	chomp;
	if (/^#(?:project|atf|link)/) {
	    @current_protocols = () if /^#project/;
	    if (/^#link/) {
		if (/:\s+def/) {
		    my ($siglum,$source) = (m/def\s+(\S+)\s+=\s+(\S+)/);
		    if ($siglum) {
			$sigla{$siglum} = $source;
		    } else {
			warn "$input:$.: malformed #link: def\n";
		    }
		}
	    } else {
		push @current_protocols, $_;
	    }
	} elsif (s/^\&//) {
	    $current_composite_link = $_;
	} elsif (/^(\S+):\s+(.*)$/) {
	    my($ref,$line) = ($1,$2);
	    my($siglum,$label) = ($ref =~ m/^(.*?)(_.*)?$/);
	    if ($siglum) {
		my $source = $sigla{$siglum};
		unless ($source) {
		    my $s2 = $siglum;
		    $s2 =~ tr/₀-₉//d;
		    unless ($siglum ne $s2 && ($source = $sigla{$s2})) {
			warn "$input:$.: bad siglum $siglum\n";
		    }
		}
		$source = $siglum unless $source;
		push @input, [ [ @current_protocols ], $current_composite_link ,  $source , $label , $line ];
	    } else {
		warn "$input:$.: no siglum in line\n";
	    }
	} else {
	    warn "$input:$.: bad start-of-line\n";
	}
    }
    close(IN);
}

1;
