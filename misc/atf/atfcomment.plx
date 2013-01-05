#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use Getopt::Long;
my $comments = '';
my %comments = ();
my $curr_id = undef;
my $status = 0;
my $waiting = 0;

GetOptions('comments:s'=>\$comments,
    );

open(C,$comments) 
    || die "atfcomment.plx: comments file `$comments' not readable\n";
while (<C>) {
    if (s/^([A-Z]\d+)\s+//) {
	my $id = $1;
	s/^/#/ unless /^\#/;
	push @{$comments{$id}}, $_;
    } else {
	warn "atfcomment.plx: bad format: comment should start with ID\n";
	++$status;
    }
}
close(C);

exit(1) if $status;

while (<>) {
    s/^\x{ef}\x{bb}\x{bf}//; # remove BOMs
    if (/^\&(\S+)/) {
	$curr_id = $1;
	$waiting = 1 if $comments{$curr_id};
	print;
    } elsif ($waiting && !/^\#/) {
	$waiting = 0;
	print @{$comments{$curr_id}};
       	print;
    } else {
	print;
    }
}

1;
