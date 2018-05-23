#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

binmode STDIN, ':utf8';
binmode STDOUT, ':utf8';
binmode STDERR, ':utf8';

# initially narrowly focussed script to apply warnings of type
# 00src/sux.forms:75: alt BASE e₄ should be primary a
# to file named in message and write to STDOUT.
# log file read from STDIN

my $curr_file = '';
my @lines = ();

my %log = ();
while (<>) {
    if (/(.*?):(.*?): alt BASE (\S+) should be primary (\S+)\s*$/) {
	my($file,$line,$alt,$pri) = ($1,$2,$3,$4);
	fixbase($file,$line,$alt,$pri);
    } elsif (/^(.*?):(.*?): BASE (\S+) should be (\S+)\s*$/) {
	my($file,$line,$alt,$pri) = ($1,$2,$3,$4);
	fixbase($file,$line,$alt,$pri);
    } elsif (/^(.*?):(.*?): \(bases\) compound (\S+) should be (\S+)\s*$/) {
	my($file,$line,$alt,$pri) = ($1,$2,$3,$4);
	fixbase2($file,$line,$alt,$pri);
    } elsif (/^(.*?):(.*?): \(bases\) sign name '(\S+)' should be '(\S+)'\s*$/) {
	my($file,$line,$alt,$pri) = ($1,$2,$3,$4);
	fixbase2($file,$line,$alt,$pri);
    } else {
	warn "nothing to do with $_";
    }
}
close_and_dump() if $curr_file;

sub fixbase {
    my($f,$l,$a,$p) = @_;
    open_and_load($f) unless $f eq $curr_file;
    my $aQ = quotemeta($a);
    unless ($lines[$l-1] =~ s#/$aQ\s#/$p #g) {
	my $err_l = $l - 1;
	my $eline = $lines[$l-1]; chomp($eline);
	warn "no /$a in $curr_file\:$err_l: $eline\n";
    } else {
	warn "fixing /$a to /$p\n";
    }
}

sub fixbase2 {
    my($f,$l,$a,$p) = @_;
    open_and_load($f) unless $f eq $curr_file;
    my $aQ = quotemeta($a);
#    my $b = '';
#    if ($lines[$l-1] =~ m#/(\S+)#) {
#	$b = $1;
#    } else {
#	$b = $lines[$l-1];
    #    }
    my $orig = $lines[$l-1];
    my $bound = '[-.{} ();,]';
    if ($lines[$l-1] =~ /^\@form/) {
	my ($prebase,$base,$postbase) = $lines[$l-1] =~ m#^(\@form\s+\S+\s+.*?)/(\S+)\s+(.*)\s*$#;
	unless ($base =~ s#(^|$bound)$aQ($bound|$)#$1$p$2#g) {
	    my $err_l = $l - 1;
	    my $eline = $lines[$l-1]; chomp($eline);
	    warn "no $a in $curr_file\:$err_l: $eline\n";
	} else {
	    $lines[$l-1] = "$prebase/$base $postbase\n";
	    warn "---\nin: ${orig}ou: $lines[$l-1]---\n";	
	}
    } else {
	unless ($lines[$l-1] =~ s#(^|$bound)$aQ($bound|$)#$1$p$2#g) {
	    my $err_l = $l - 1;
	    my $eline = $lines[$l-1]; chomp($eline);
	    warn "no $a in $curr_file\:$err_l: $eline\n";
	} else {
	    warn "---\nin: ${orig}ou: $lines[$l-1]---\n";	
	}
    }
}

sub open_and_load {
    my $f = shift;
    close_and_dump() if $curr_file;
    if (open(F,$f)) {
	$curr_file = $f;
	@lines = (<F>);
	warn "$curr_file has $#lines lines\n";
    } else {
	die "can't open err forms file $f\n";
    }
}

sub close_and_dump {
    open(N,">$curr_file.new"); print N @lines; close(N);
    close(F);
}

1;
