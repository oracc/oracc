#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;

binmode STDIN, ':utf8';
binmode STDOUT, ':utf8';
binmode STDERR, ':utf8';

# fix a variety of errors as logged by BaseC and CBD/Validate
# applied to file/line named in message and write to STDOUT.
# log file read from STDIN

my $bound = '[-.{} ();,]';
my $curr_file = '';
my @lines = ();
my $verbose = 0;

my %log = ();
while (<>) {
    if (/(.*?):(.*?): alt BASE (\S+) should be primary (\S+)\s*$/) {
	my($file,$line,$alt,$pri) = ($1,$2,$3,$4);
	# fixbase($file,$line,$alt,$pri);
	fix_in_base($file,$line,$alt,$pri);
    } elsif (/^(.*?):(.*?): form's BASE (\S+) should be (\S+)\s*$/) {
	my($file,$line,$alt,$pri) = ($1,$2,$3,$4);
	# fixbase($file,$line,$alt,$pri);
	fix_in_form($file,$line,$alt,$pri);
    } elsif (/^(.*?):(.*?): \(bases\) compound (\S+) should be (\S+)\s*$/) {
	my($file,$line,$alt,$pri) = ($1,$2,$3,$4);
	# fixbase2($file,$line,$alt,$pri);
	fix_in_base($file,$line,$alt,$pri);
	fix_in_form($file,$line,$alt,$pri);
    } elsif (/^(.*?):(.*?): \(bases\) sign name '(\S+)' should be '(\S+)'\s*$/) {
	my($file,$line,$alt,$pri) = ($1,$2,$3,$4);
	# fixbase2($file,$line,$alt,$pri);
	fix_in_base($file,$line,$alt,$pri);
	fix_in_form($file,$line,$alt,$pri);
    } elsif (/^(.*?):(.*?): \(bases\) core (\S+) of base (\S+) should be (\S+)$/) {
	my($file,$line,$core,$base,$should) = ($1,$2,$3,$4,$5);
	# fixbase3($file,$line,$base,$core,$should);
	fix_in_base($file,$line,$core,$should);
    } elsif (/^(.*?):(.*?): \(bases\).*?Q4.*?vq=(\S+?): .*?suggest (\S+)\s*$/) {
	my($file,$line,$vq,$use) = ($1,$2,$3,$4);
	# fixbase4($file,$line,$vq,$sugg);
	fix_in_base($file,$line,$vq,$use);
	fix_in_form($file,$line,$vq,$use);
    } elsif (/^(.*?):(.*?): \(bases\).*?Q1c.*?vq=(\S+?): .*?use (\S+)\s*$/) {
	my($file,$line,$vq,$use) = ($1,$2,$3,$4);
	# fixbase4($file,$line,$vq,$sugg);
	fix_in_base($file,$line,$vq,$use);
	fix_in_form($file,$line,$vq,$use);
    } else {
	warn "nothing to do with $_"
	    unless /\(bases\)/ || /cbdpp/;
    }
}
close_and_dump() if $curr_file;

sub fix_in_base {
    my($f,$l,$bad,$good) = @_;
    my $ln = fix_get_line($f,$l);
    if ($ln =~ /^\@bases/) {
	my @bases = fix_bases_list($ln);
#	use Data::Dumper; warn Dumper \@bases;
	my $badQ = quotemeta($bad);
	my $nfix = 0;
#	warn "elements in bases = ", $#bases+1, "\n";
	foreach my $b (@bases) {
	    next if $$b[0] eq $good;
	    $nfix += fix_b($b,$badQ,$good);
	}
	if ($nfix) {	    
	    fix_set_line($l,fix_bases_back(@bases));
	} else {
	    warn "$f:$l: $bad not found in any primary base\n";
	}
    } else {
	warn "$f:$l: not a \@bases line\n" if $verbose;
    }
}

sub fix_b {
    my ($b,$badQ,$good) = @_;
    my $bb0 = $$b[0];
    my $n = 0;
    $good = fix_hide_bound($good);
    ++$n while $bb0 =~ s/(^|$bound)$badQ($|$bound)/$1$good$2/;
    $bb0 = fix_show_bound($bb0);
    warn "base $$b[0] => $bb0\n" if $n;
    $$b[0] = $bb0 if $n;
    $n;
}

sub fix_in_form {
    my($f,$l,$bad,$good) = @_;
    my $ln = fix_get_line($f,$l);
    if ($ln =~ /^\@form/) {
	my ($prebase,$base,$postbase) = $ln =~ m#^(\@form\s+\S+\s+.*?)/(\S+)\s+(.*)\s*$#;
	my $orig_base = $base;
	my $badQ = quotemeta($bad);
	my $nfix = 0;
	my $n = 0;
	$good = fix_hide_bound($good);
	do {
	    $n = ($base =~ s#(^|$bound)$badQ($|$bound)#$1$good$2#);
	    ++$nfix if $n;
	} while ($n);
	$base = fix_show_bound($base);
	if ($nfix) {
	    warn "base /$orig_base => /$base\n";
	    fix_set_line($l,"$prebase/$base $postbase");
	} else {
	    warn "$f:$l: $bad not found as /BASE in form (base=$base)\n";
	}
    } else {
	warn "$f:$l: not a \@form line\n" if $verbose;
    }
}
sub fix_get_line {
    my($f,$l) = @_;
    open_and_load($f) unless $f eq $curr_file;
    $lines[$l-1];
}
sub fix_set_line {
    my($l,$ln) = @_;
    chomp($ln);
    warn "setting to $ln\n";
    $lines[$l-1] = "$ln\n";
}
sub fix_bases_list {
    my $ln = shift;
    chomp $ln;
    $ln =~ s/^\@bases\S*\s+//;
    my @b = split(/;\s+/, $ln);
    my @bb = ();
    foreach my $b (@b) {
	my $p = $b;
	my $a = '';
	if ($p =~ s/\s+(.*)$//) {
	    $a = $1;
	}
	push @bb, [ $p, $a ];
    }
    return @bb;
}
sub fix_bases_back {
    my @bb = @_;
    my $ln = '@bases ';
    my @bbb = ();
    foreach my $bb (@bb) {
	my $bbb = "@$bb";
	$bbb =~ s/\s*$//;
	push @bbb, $bbb;
    }
    $ln .= join('; ', @bbb);
    $ln;
}

sub fix_hide_bound {
    my $tmp = shift;
    $tmp =~ tr/{}()/\000\001\002\003/;
    $tmp;
}

sub fix_show_bound {
    my $tmp = shift;
    $tmp =~ tr/\000\001\002\003/{}()/;
    $tmp;
}

# Safe because tied to the / in a @form
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
