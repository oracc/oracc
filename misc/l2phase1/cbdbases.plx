#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
use lib '/usr/local/oracc/lib';
use ORACC::SL::Base;
use Getopt::Long;
use Pod::Usage;
use Data::Dumper;
binmode STDERR, ':utf8'; binmode STDIN,  ':utf8'; binmode STDOUT, ':utf8';

use constant NEW_PASS=>0;
use constant CBD_PASS=>1;

my $bases = '';
my $began_merging = 0;
my $bstatus = 0;
my $cbd = '';
my $check = '';
my $found_bases = 1;
my $from_forms = 0;
my %current_mergers = ();
my @newbases = ();
my $rewrite = 0;
my $verbose = 0;

GetOptions(
    "bases:s"=>\$bases,
    "cbd:s"=>\$cbd,
    "check:s"=>\$check,
    "from-forms"=>\$from_forms,
    "rewrite"=>\$rewrite,
    "verbose"=>\$verbose,
    ) || pod2usage(1);

my %cbd_bases = ();
my $entry = '';
my $in_bases = 0;
my %mergers = ();

open LOG, ">cbdbases.log";
ORACC::SL::Base::init();
if ($check) {
    do_check_bases($check);
    exit $bstatus;
} elsif ($rewrite) {
    my %entries = ();
    while (<>) {
	if (/sma:\s+\S+?=(.*?) has new BASE (\S+)$/) {
	    ++${$entries{$1}}{$2};
	} elsif (/sma: (\S+\s+)?(.*?)=(.*?) no bases found in form$/) {
	    my($base,$form,$cfgw) = ($1,$2,$3);
	    if ($base) {
		++${$entries{$3}}{$1};
	    } else {
		++${$entries{$3}}{$2};
	    }
	} elsif (/sma: \s+(.*?)=(.*?) no (?:bases|parse) found/) {
	    ++${$entries{$2}}{$1};
	}
    }
    foreach my $e (sort keys %entries) {
	print "\@entry $e\n\@bases ";
	print join('; ',sort keys %{$entries{$e}}),"\n";
	print "\n";
    }
    exit 0;
} else {
    process_bases($bases);
    merge_bases($cbd);
}
1;

###########################################################################

sub
check_bases {
    my $tmp = shift;
    chomp($tmp);
    $tmp =~ s/\s+\(.*?\)(;|$)/$1/g;
    $tmp =~ tr/°·//d;
    $tmp =~ s/\*\S+(?:\s+|$)//g;
#    $tmp =~ s/\s+\((.*?)\)/, $1, /;
    $tmp =~ tr/;/\cA/;
    $tmp =~ s/\s*\cA\s*/\cA/g;
    my %b_seen = ();
    foreach my $b (grep(length,split(/\cA/,$tmp))) {
	my $b_sig = ORACC::SL::Base::check("$check:$.:$b",$b);
	++$bstatus and next if $b_sig =~ /q0[01]/;
	push @{$b_seen{$b_sig}}, $b;
    }
    foreach my $b_sig (sort keys %b_seen) {
	if ($#{$b_seen{$b_sig}} > 0) {
	    warn "$check:$.: $b_sig occurs as @{$b_seen{$b_sig}}\n";
	}
    }
}

sub
do_check_bases {
    my($input) = @_;
    open(M,$input) || die "cbdbases.plx: -check FILE\n";
    while (<M>) {
	if (/^\@entry\*?\s+(.*?)$/) {
	    my $this_entry = $1;
	    warn "no bases for $entry\n"
		unless $found_bases;
	    $found_bases = $in_bases = 0;
	    $entry = $this_entry;
	} elsif (s/^\@bases\s+//) {
	    $found_bases = $in_bases = 1;
	    check_bases($_);
	} elsif (s/^\s+// && $in_bases) {
	    check_bases($_);
	} else {
	    $found_bases = 1 if /^\@parts/;
	    $in_bases = 0;
	}
    }
    close(M);
    warn "no bases for $entry\n"
	unless $found_bases;
}

sub
process_bases {
    my($input) = @_;
    open(M,$input) || die "cbdbases.plx: -bases=FILE -cbd=FILE needed\n";
    while (<M>) {
	if (/^\@entry\*?\s+(.*?)$/) {
	    $in_bases = 0;
	    $entry = $1;
	} elsif (s/^\@bases\s+//) {
	    $in_bases = 1;
	    register_bases($_);
	} elsif (s/^\s+// && $in_bases) {
	    register_bases($_);
	} else {
	    $in_bases = 0;
	}
    }
    close(M);
}
sub
merge_bases {
    my($input,$pass) = @_;
    my $merging = 0;
    open(M,$input) || die;
    while (<M>) {
	if (/^\@entry\*?\s+(.*?)$/) {
	    $in_bases = 0;
	    $entry = $1;
	    $entry =~ s/\s+\[/[/;
	    $entry =~ s/\]\s+/]/;
	    $merging = defined $mergers{$entry};
	    logit("merging on for $entry\n") if $merging;
	    print;
	} elsif (s/^\@bases\s+//) {
	    $in_bases = 1;
	    if ($merging) {
		perform_merging($entry);
	    } else {
		print "\@bases ", $_;
	    }
	} elsif (s/^\s+(?>\S)// && $in_bases) {
	    if ($merging) {
		perform_merging($entry);
	    } else {
		print "\t",$_;
	    }
	} else {
	    $in_bases = 0;
	    if ($merging) {
		finish_merging($entry);
		$merging = 0;
	    }
	    print;
	}
    }
    close(M);
}

sub
perform_merging {
    chomp;
    if (!$began_merging) {
	$began_merging = 1;
	@current_mergers{@{$mergers{$entry}}} = ();
#	logit(Dumper(\%current_mergers));
    }
    my @tgroups = ();
    tr/°·//d;
    s/\*\S+\s+//g;
    foreach my $tlit_group (split(/;/,$_)) {
	$tlit_group =~ /(\S+)/;
	my $tlit = $1;
	my @tlit_bits = ();
	foreach my $k (keys %current_mergers) {
	    if (ORACC::SL::Base::same_tlit("$cbd:$.",$k,$tlit)) {
		if ($k eq $tlit) {
		    warn "skipping reading $k = $tlit\n" if $verbose;
		} else {
		    warn "adding reading $k to $tlit\n" if $verbose;
		    if ($tlit_group =~ /\)$/) {
			$tlit_group =~ s/\)/, $k)/;
		    } else {
			$tlit_group .= " ($k)";
		    }
		}
		delete $current_mergers{$k};
	    }
	}
	push @tgroups, $tlit_group;
    }
    $tgroups[0] =~ s/^\s+//;
    push @newbases, join('; ', @tgroups);
}

sub
finish_merging {
    $began_merging = 0;
    if ($verbose) {
	foreach my $n (@newbases) {
	    warn("adding new base $n\n");
	}
    }
    if (scalar keys %current_mergers) {
	print "\@bases @newbases; ", join('; ', keys %current_mergers), "\n";
	%current_mergers = ();
    } else {
	print "\@bases @newbases\n";
    }
    @newbases = ();
}

sub
register_bases {
    my $tmp = shift;
    chomp($tmp);
    $tmp =~ tr/°·//d;
    $tmp =~ s/\*\S+\s+//;
    $tmp =~ s/\s+\((.*?)\)/, $1, /;
    $tmp =~ tr/,;/\cA\cA/;
    $tmp =~ s/\s*\cA\s*/\cA/g;
    push @{$mergers{$entry}}, grep(length,split(/\cA/,$tmp));
}

sub
logit {
    print LOG @_;
}

1;
