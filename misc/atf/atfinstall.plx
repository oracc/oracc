#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

my $atfinput = '';
my $printing = 0;
my $P = '';

open(A,'01bld/atfsources.lst') || die "$0: I only work with 01bld/atfsources.lst but it's not there\n";
while (<A>) {
    chomp;
    split_file($_);
}
close(A);

sub split_file {
    $atfinput = shift;
    open(F,$atfinput) || die "$0: can't read $atfinput\n";
    $P = '';
    while (<F>) {
	s/^\x{ef}\x{bb}\x{bf}//; # remove BOMs
	if (/^&([PQX]\d+)/) {
	    finish_text($1);
	    print;
	} elsif (/^&/) {
	    chomp;
	    warn "atfsplit.plx: bad &-line '$_'" unless /^&\s/;
	    $printing = 0;
	} else {
	    print if $printing;
	}
    }
    finish_text('');
    close(F);
}

sub finish_text {
    my $argP = shift @_;
    print "\n" if $printing;
    if ($P ne $argP) {
	my $last_P = $P;
	$P = $argP;
	close OUT;
	my $dname = $P;
	$dname =~ s/...$//;
	$dname = "01bld/$dname/$P";
	system("mkdir -p $dname") unless -d $dname;
	my $fname = "$dname/$P.atf";
	open(OUT,">$fname") || die "atfsplit.plx: can't write '$fname'\n";
	select OUT;
	print "##file $atfinput\n";
	print "##line $.\n";
	$printing = 1;
	chmod 0664, $fname;
    }
}

1;
