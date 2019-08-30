#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC'}/lib";
use ORACC::CBD::History;
use ORACC::L2GLO::Util;
use ORACC::Texts::Util;

use Data::Dumper;

# Under the stash/align system, $w2l here must be init-locdata
my $w2l = shift @ARGV;
$w2l = `cbdstash.plx locdata init`;
my @w = ();
if ($w2l =~ /xz$/) {
    @w = `unxz -c $w2l`;
} else {
    @w = `cat $w2l`;
}
chomp @w;
# Now we have a hash of form=inst from the orig data
my %orig_inst = wid2lem_inst($w2l,\@w);

# Get the form=inst pairs that don't parse as a result of the edits
my @bad = `ox -CD 00atf/*.atf 2>/dev/null | wid2lem -s | grep BAD\$ | grep -F \'[\' | cut -f2`; chomp @bad;

print join("\n", @bad), "\n";

# Now we need a hash of the changes made in the history file
my %h = history_map();
open(H,'>history.dump'); print H Dumper \%h; close(H);

# Now for each of the bad form=inst pairs look it up in orig_inst to
# get the orig sig and then map that to the new sig via the history
# map:

foreach my $b (@bad) {
    my $n = '';
    if ($orig_inst{$b}) {
	my $osig = $orig_inst{$b};
	my %p = parse_sig($osig);
	my $ocore = "$p{'cf'}\[$p{'gw'}//$p{'sense'}\]$p{'pos'}'$p{'epos'}";
	if ($h{$ocore}) {
	    $n = $h{$ocore};
	} else {
	    $ocore = "$p{'cf'}\[$p{'gw'}\]$p{'pos'}";
	    if ($h{$ocore}) {
#		warn "$ocore ==>> $h{$ocore}\n";
		$n = $h{$ocore};
	    } else {
		warn "osig = $ocore not found\n";
	    }
	}
    } else {
	warn "$0: bad form=inst $b not found in original location data\n";
    }
    if ($n) {
	my($f,$i) = ($b =~ /^(.*?)=(.*?)$/);
	my %n = parse_sig($n);
	my $s = $n{'sense'} || $n{'gw'};
	my $ninst = "$n{'cf'}\[$s\]";
	if ($f) {
	    print "$f\t$i\t$ninst\n";
	} else {
	    print "\t$b\t$ninst\n";
	}
    }
}

1;
