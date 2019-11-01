#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

# Read the output of cbd-lem-fix.plx and apply it to the .atf files

###
### Exit status =
###	0 for changes applied successfully
###	1 for error
###	2 for no changes required
###

use lib "$ENV{'ORACC'}/lib";

my $changes = 0;
my $curr_file = '';
my $file = '';
my $filecount = 0;
my $line = 0;
my @lines = ();
my $project = `oraccopt`;
my $status = 0;

while (<>) {
    chomp;
    my($loc,$wid,$inst,$new,$from) = split(/\t/,$_);
    ($file,$line) = ($loc =~ /^(.*?):(.*?):/);
    if ($file ne $curr_file) {
	if ($#lines >= 0) {
	    close(F);
	    unless ($status) {
		open(F,">$curr_file") || die "$0: can't open $file to write changes\n";
		print F @lines;
		close(F);
	    } else {
		warn "$0: changes not written to $curr_file because of errors\n";
	    }
	}
	open(F,$file) || die "$0: can't open $file to read input.\n";
	$curr_file = $file;
	@lines = (<F>);
	$status = 0;
	++$filecount;
    }
#    warn "[1]line = $lines[$line]\n";
    while ($line <= $#lines && $lines[$line] !~ /^\S+\.\s/) {
#	warn "[2]line = $lines[$line]\n";
	last if $lines[$line] =~ /^\#lem:/;
	++$line;
#	warn "[3]line = $lines[$line]\n";
    }
#    warn "[4]line = $lines[$line]\n";
    if ($lines[$line] =~ /^\#lem:/) {
	my $w = $wid;
	$w =~ s/^.*?\.(\d+)$/$1/;
#	warn "[5]trying wid $w\n";
	my $l = $lines[$line];
	$l =~ s/^\#lem:\s+//; chomp $l;
	my @l = split(/;\s+/,$l);
	# use contains rather than eq because parasyntax notations
	my $found_w = find_inst($inst,$w,@l);
	if ($found_w) {
	    if ($found_w ne $w) {
		warn "$curr_file:$line: inst $inst found at $found_w not $w; fixing anyway.\n";
		$w = $found_w;
	    }
	    # warn "$curr_file:$line: found $inst; replacing it with $new\n";
	    $l[$w-1] =~ s/\Q$inst/$new/;
	    ++$changes;
	} else {
	    warn "$curr_file:$line: lem\[$w\] expected inst $inst but not found in line\n";
	    warn "\tline=$l\n";
	    ++$status;
	}
	$l = '#lem: '.join('; ', @l)."\n";
	$lines[$line] = $l;
    } else {
	die "$0: something wrong with $file:$line -- no #lem line following\n";
    }
}
close(F);

unless ($status) {
    open(F,">$curr_file") || die "$0: can't open $file to write changes\n";
    print F @lines;
    close(F);
    print "$0: made $changes changes in $filecount files in project $project\n";
} else {
    warn "$0: changes not written to $curr_file because of errors\n";
}

sub find_inst {
    my($inst,$w,@l) = @_;
    while ($w > 0) {
	if ($l[$w-1] && $l[$w-1] =~ /\Q$inst/) {
	    return $w;
	}
	--$w;
    }
    0
}

1;
