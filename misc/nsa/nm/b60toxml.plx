#!/usr/bin/perl
use warnings; use strict;
use PSD::SUM::CC;

PSD::SUM::CC::load();

my $ns_uri="http://psd.museum.upenn.edu/nm";

open(STDIN,'base-60.txt');
open(STDOUT,'>base-60.xml');
process_base();
close(STDOUT);
close(STDIN);

sub
process_base {
    my $found_system = 0;
    while (<>) {
	if (s/^\@system\s+(\S+)\s*//) {
	    my $system = $1;
	    my %args = ();
	    foreach my $a (qw/time place aev mev/) {
		my($val) = /$a=(\S+)/;
		$args{$a} = $val || '';
	    }
	    print <<EOS;
<system xmlns="$ns_uri"
    n="$system" time="$args{'time'}" place="$args{'place'}"
    aev="$args{'aev'}" mev="$args{'mev'}"
EOS
	    print ">";
            $found_system = 1;
	    last;
	}
    }
    die "never found a \@system\n" unless $found_system;
    while (<>) {
	next if /^\s*$/ || /^\#/;
	if (s/^\@x\s+//) {
	    chomp;
	    my $cc = PSD::SUM::CC::atf_xml($_);
	    print "<example cc=\"$cc\" atf=\"$_\"\n/>";
	    next;
	} elsif (s/^\@step\s+//) {
	} elsif (/^\@examples/) {
	} else {
	    my($count,$unit,$range,$aev,$glyph)
		= (/^(.*?)\s+(.*?)\s+(.*?)\s+(.*?)\s+(.*?)$/);
	    my($exp,$atf,$mod) = ($glyph =~ /^(.*)=(\S+)(.*)$/);
	    $exp = $exp || $glyph;
	    $atf = $atf || $glyph;
	    $exp .= $mod if $mod;
	    $atf .= $mod if $mod;
	    my(@vals);
	    if ($range =~ /^(\d+)\.\.(\d+)$/) {
		my($min,$max) = ($1,$2);
		my $j = 0;
		for (my $i = $min; $i <= $max; ++$i) {
		    $vals[$j++] = $i;
		}
	    } elsif ($range =~ /^(\d+)\.\.n=(.*?)$/) {
		my($min,$n) = ($1,$2);
		my $j = 0;
		for (my $i = $min; $i <= $n; ++$i) {
		    $vals[$j++] = $i;
		}
	    } elsif ($range =~ /^n=(.*?)$/) {
		my $n = $1;
		my $j = 0;
		for my $i (split(/,/,$n)) {
		    $vals[$j++] = $i;
		}
	    } else {
		$vals[0] = $range;
	    }
#	    print <<EOS;
#	    <step count="$count" unit="$unit" aev="$aev"
#		EOS
	    my $open_step_instance;
	    foreach my $v (@vals) {
		my($patf,$pexp) = ($atf,$exp);
		my $value;
		if ($aev =~ /1\/n/) {
		    my $mult = $aev;
		    $mult =~ s/1\/n×//;
		    $value = $mult * $v;
		    $value = "1/$value";
		} elsif ($aev =~ m,(\d+)/(\d+),) {
		    my ($top,$bot) = ($1,$2);
		    $value = $top * $v;
		    $value = "$value/$bot";
		} else {
		    $value = $aev * $v;
		}
		$patf =~ s/\#/$v/x;
		$pexp =~ s/\#/$v/x;
		my $cc = PSD::SUM::CC::atf_xml($pexp);
		print "/" if $open_step_instance;
		print <<EOS;
><step-instance count="$v" aev="$value" atf="$patf" glyph="$pexp" cc="$cc"
EOS
		++$open_step_instance;
	    }
#	    print "/></step>";
    }
    print "</system>";
}
