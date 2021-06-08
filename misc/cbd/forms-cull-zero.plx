#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use ORACC::CBD::Util;
my $acd_rx = $ORACC::CBD::acd_rx;

my $curr_entry = '';
my %forms = ();
my @inst = `form-inst.sh`; chomp(@inst);
my %inst = (); @inst{@inst} = (); # print Dumper \%inst; exit;
my $lang = '';

system 'mkdir', '-p', '00any';
my $newiso = `date +\%Y-\%m-\%d`; chomp $newiso;
my $cullfile = "00any/cull-$newiso.forms";
open(CULL,">$cullfile") || die "$0: can't write culled forms to $cullfile\n";

my %part_forms = ();
my @input = ();
while (<>) {
    push @input, $_;
    chomp;
    if (/^\@form\S*\s+(\S+)/) {
	my $f = $1;
	if ($f =~ /_/) {
	    my @f = split(/_/, $f);
	    @part_forms{@f} = ();
	}
    }
}

open(F,'>F'); print F join("\n", keys %part_forms); close(F);

foreach (@input) {
    if (/^$acd_rx\@entry/) {
	warn "$.: missing \@end entry\n" if scalar keys %forms > 0;
	$curr_entry = $_;
	$curr_entry =~ s/^.*\@entry\s+(.*?)\s*$/$1/;
	$curr_entry =~ s/\s+\[(.*?)\].*$/[$1]/;
	%forms = ();
    } elsif (!/_/ && /^\@form/) { # ignore compound forms because they are always zero via this method
	chomp;
	my $bang = '';
	$bang = '!' if /^\@form\!/;
	s/^\@form\S*\s+//;
	s/\s+/ /;
	s/\s*$//;
	my ($f) = (/^(\S+)/);
	my $k = "$lang:$f=$curr_entry"; # warn "k=$k\n";
	if (exists($part_forms{$f}) || exists($inst{$k})) {
	    $forms{$f} = "\@form$bang $_" unless $forms{$f};
	} else {
	    $forms{$f} = "-\@form $_";
	}
	$_ = undef;
    } elsif (/^$acd_rx\@sense/ || /^\@end/) {
	foreach my $f (sort keys %forms) {
	    if ($forms{$f} =~ s/^-//) {
		print CULL "$curr_entry\t$forms{$f}\n";
	    } else {
		print "$forms{$f}\n";
	    }
	}
	%forms = ();
    } elsif (/\@lang\s+(\S+)/) {
	$lang = $1;
	$lang = 'sux' if $lang eq 'qpn';
    }
    print if $_;
}

close(CULL);

1;
