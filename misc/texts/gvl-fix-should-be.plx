#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

my $original_qualifier = 0;

GetOptions(
    o=>\$original_qualifier
    );

my $curr_file = '';
my @lines = ();

while (<>) {
    if (/(.*?):(.*?):.*?\(gvl\)\s+(\S+): should be (\S+)\s*$/) {
	my($file,$line,$bad,$good) = ($1,$2,$3,$4);
	$good = reset_good_qual($good) if $original_qualifier;
	fix_in_atf($file,$line,$bad,$good);
	#    } elsif (/<=/ && /(.*?):(.*?): (\S+): should be (\S+) /) {
    } elsif (/^(.*?):(.*?): (\S+): should be (\S+)$/
	     || /^(.*?):(.*?):(?: \(gvl\)?) (\S+): should be (\S+) \[/) {
	my($file,$line,$alt,$pri) = ($1,$2,$3,$4);
	$pri = reset_good_qual($pri) if $original_qualifier;
	fix_in_atf($file,$line,$alt,$pri);
    }
}
close_and_dump() if $curr_file;

####################################################################################

sub fix_in_atf {
    my($f,$l,$b,$g) = @_;
    my $ln = fix_get_line($f,$l);
    #warn "fixing $b to $g\n";
    my $ok = $ln =~ s/\Q$b/$g/;
    unless ($ok) {
	my $bx = $b; $bx =~ s/\(.*$/(/;
	my $gx = $g; $gx =~ s/\(.*$/(/;
	$ok = $ln =~ s/\Q$bx/$gx/;
    }
    if ($ok) {
	fix_set_line($l,$ln);
    } else {
	warn "$f:$l: $b not found; not fixed to $g\n";
    }
}

sub fix_get_line {
    my($f,$l) = @_;
    open_and_load($f) unless $f eq $curr_file;
    $l =~ s/:[PX].*$//;
    $lines[$l-1];
}

sub fix_set_line {
    my($l,$ln) = @_;
    chomp($ln);
    warn "setting to $ln\n";
    $l =~ s/:[PX].*$//;
    $lines[$l-1] = "$ln\n";
}

sub reset_good_qual {
    my $g = shift;
    my $o = $g;
    my $l = $_; chomp($l);
    warn "reset_good_qual called on $l\n";
    if (/\[(.*?) <= (.*?)\]$/) {
	my($from,$to) = ($2,$1);
	if ($from =~ /^LAK\d/) {
	    $g =~ s/\((.*?)\)$/($from)/;
	    warn "reset_good_qual resetting $o to $g\n";
	} else {
	    my $x = $g;
	    $x =~ s/\((.*?)\)$/($from)/;
	    warn "reset_good_qual not resetting $o to $x\n";
	}
    }
    return $g;
}

sub open_and_load {
    my $f = shift;
    close_and_dump() if $curr_file;
    if (open(F,$f)) {
	$curr_file = $f;
	@lines = (<F>);
	# warn "$curr_file has $#lines lines\n";
    } else {
	die "can't open err forms file $f\n";
    }
}

sub close_and_dump {
    open(N,">$curr_file.new"); print N @lines; close(N);
    close(F);
}

1;
