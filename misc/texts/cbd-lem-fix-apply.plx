#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

###
### Exit status =
###	0 for changes applied successfully
###	1 for error
###	2 for no changes required
###

use lib "$ENV{'ORACC'}/lib";

my $curr_file = '';
my $file = '';
my $line = 0;
my @lines = ();

while (<>) {
    chomp;
    my($loc,$wid,$inst,$new,$from) = split(/\t/,$_);
    ($file,$line) = ($loc =~ /^(.*?):(.*?):/);
    if ($file ne $curr_file) {
	if ($#lines >= 0) {
	    close(F);
	    open(F,">$curr_file") || die "$0: can't open $file to write changes\n";
	    print F @lines;
	    close(F);
	}
	open(F,$file) || die "$0: can't open $file to read input.\n";
	$curr_file = $file;
	@lines = (<F>);
    }
    warn "[1]line = $lines[$line]\n";
    while ($line <= $#lines && $lines[$line] !~ /^\S+\.\s/) {
	warn "[2]line = $lines[$line]\n";
	last if $lines[$line] =~ /^\#lem:/;
	++$line;
	warn "[3]line = $lines[$line]\n";
    }
    warn "[4]line = $lines[$line]\n";
    if ($lines[$line] =~ /^\#lem:/) {
	my $w = $wid;
	$w =~ s/^.*?\.(\d+)$/$1/;
	warn "trying wid $w\n";
    } else {
	die "$0: something wrong with $file:$line -- no #lem line following\n";
    }
}

close(F);
#open(F,">$curr_file") || die "$0: can't open $file to write changes\n";
print F @lines;
#close(F);

1;
