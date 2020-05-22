#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;

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
    } elsif (/^(.*?):(.*?): \(bases\) core (\S+) of base (\S+) should be (\S+)$/) {
	my($file,$line,$core,$base,$should) = ($1,$2,$3,$4,$5);
	fixbase3($file,$line,$base,$core,$should);
    } elsif (/^(.*?):(.*?): \(bases\).*?Q4.*?vq=(\S+?): .*?suggest (\S+)\s*$/) {
	my($file,$line,$vq,$sugg) = ($1,$2,$3,$4);
	fixbase4($file,$line,$vq,$sugg);
    } else {
	warn "nothing to do with $_"
	    unless /\(bases\)/ || /cbdpp/;
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

sub fixbase3 {
    my($f,$l,$base,$core,$should) = @_;
    open_and_load($f) unless $f eq $curr_file;
    my $baseQ = quotemeta($base);
    my $nmatch = $lines[$l-1] =~ m/$baseQ/;
    if ($nmatch == 1) {
	my $coreQ = quotemeta($core);
	my $nbase = $base;
	my $nmatch = $nbase =~ m/$coreQ/;
	$nmatch = 0 unless $nmatch;
	if ($nmatch != 1) {
	    warn "$nmatch matches to $coreQ in $nbase, aborting fixbase3\n";
	    return;
	}
	unless ($nbase =~ s/$coreQ/$should/) {
	    warn "failed to edit $coreQ to $should in $nbase\n";
	} else {
	    warn "$base => $nbase (core=$core; should=$should)\n";
	}
	if ($lines[$l-1] =~ s/$baseQ/$nbase/) {
	    warn "$base replaced with $nbase\n";
	} else {
	    warn "$base fix to $nbase failed\n";
	}
    } elsif ($nmatch > 1) {
	warn "$base (as $baseQ) found $nmatch times in $lines[$l-1]\n";
    } else {
	warn "$base (as $baseQ) not found in $lines[$l-1]\n";
    }
}

sub fixbase4 {
    my($f,$l,$vq,$sugg) = @_;
    open_and_load($f) unless $f eq $curr_file;
    my $vqQ = quotemeta($vq);
    my $nmatch = $lines[$l-1] =~ m/$vqQ/;
    if ($nmatch == 1) {
	if ($lines[$l-1] =~ s/$vqQ/$sugg/) {
	    warn "$vq replaced with $sugg\n";
	} else {
	    warn "$vq fix to $sugg failed\n";
	}
    } elsif ($nmatch > 1) {
	warn "$vq (as $vqQ) found $nmatch times in $lines[$l-1]\n";
    } else {
	warn "$vq (as $vqQ) not found in $lines[$l-1]\n";
    }
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
