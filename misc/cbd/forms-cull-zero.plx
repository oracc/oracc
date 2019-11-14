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

my $newiso = `date +\%Y-\%m-\%d`; chomp $newiso;
my $cullfile = "00any/cull-$newiso.forms";
open(CULL,">$cullfile") || die "$0: can't write culled forms to $cullfile\n";

while (<>) {
    if (/^$acd_rx\@entry/) {
	warn "$.: missing \@end entry\n" if scalar keys %forms > 0;
	$curr_entry = $_;
	$curr_entry =~ s/^.*\@entry\s+(.*?)\s*$/$1/;
	$curr_entry =~ s/\s+\[(.*?)\].*$/[$1]/;
	%forms = ();
    } elsif (!/_/ && s/^\@form\s*//) { # ignore compound forms because they are always zero via this method
	chomp;
	s/\s+/ /;
	s/\s*$//;
	my ($f) = (/^(\S+)/);
	my $k = "$lang:$f=$curr_entry"; # warn "k=$k\n";
	if (exists $inst{$k}) {
	    $forms{$f} = $_ unless $forms{$f};
	} else {
	    $forms{$f} = "-$_";
	}
	$_ = undef;
    } elsif (/^$acd_rx\@sense/ || /^\@end/) {
	foreach my $f (sort keys %forms) {
	    if ($forms{$f} =~ s/^-//) {
		print CULL "$curr_entry\t\@form $forms{$f}\n";
	    } else {
		print "\@form $forms{$f}\n";
	    }
	}
	%forms = ();
    } elsif (/\@lang\s+(\S+)/) {
	$lang = $1;
    }
    print if $_;
}

close(CULL);

1;
