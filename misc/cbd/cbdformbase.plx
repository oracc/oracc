#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;

binmode STDIN, ':utf8';
binmode STDOUT, ':utf8';
binmode STDERR, ':utf8';

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::CBD::Bases;

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
	fix_in_form($file,$line,$alt,$pri);
    } elsif (/^(.*?):(.*?): form's BASE (\S+) should be (\S+)\s*$/) {
	my($file,$line,$alt,$pri) = ($1,$2,$3,$4);
	fix_in_form($file,$line,$alt,$pri);
    } elsif (/^(.*?):(.*?): \(atf error\) \(gvl\) \[sb[25]\] (\S+?): should be (\S+)$/
	|| /^(.*?):(.*?): \(atf error\) \(gvl\) \[sb[25]\] (\S+?): should be (\S+) .*$/) {
	my($file,$line,$alt,$pri) = ($1,$2,$3,$4);
	fix_in_form_form($file,$line,$alt,$pri);
    } elsif (/^(.*?):(.*?): \(bases\) compound (\S+) should be (\S+)\s*$/) {
	my($file,$line,$alt,$pri) = ($1,$2,$3,$4);
	fix_in_base($file,$line,$alt,$pri);
	fix_in_form($file,$line,$alt,$pri);
    } elsif (/^(.*?):(.*?): \(bases\) sign name '(\S+)' should be '(\S+)'\s*$/) {
	my($file,$line,$alt,$pri) = ($1,$2,$3,$4);
	fix_in_base($file,$line,$alt,$pri);
	fix_in_form($file,$line,$alt,$pri);
    } elsif (/^(.*?):(.*?): \(bases\) core (\S+) of base (\S+) should be (\S+)$/) {
	my($file,$line,$core,$base,$should) = ($1,$2,$3,$4,$5);
	fix_in_base($file,$line,$core,$should);
    } elsif (/^(.*?):(.*?): \(bases\).*?Q4.*?vq=(\S+?): .*?suggest (\S+)\s*$/) {
	my($file,$line,$vq,$use) = ($1,$2,$3,$4);
	fix_in_base($file,$line,$vq,$use);
	fix_in_form($file,$line,$vq,$use);
    } elsif (/^(.*?):(.*?): \(bases\).*?Q1c.*?vq=(\S+?): .*?use (\S+)\s*$/) {
	my($file,$line,$vq,$use) = ($1,$2,$3,$4);
	fix_in_base($file,$line,$vq,$use);
	fix_in_form($file,$line,$vq,$use);
    } elsif (/^(.*?):(.*?): \(bases\) phonetically determined BASE (.*?) should be (.*?)$/) {
	my($file,$line,$base,$should) = ($1,$2,$3,$4);
	fix_in_base($file,$line,$base,$should);
    } elsif (/^(.*?):(.*?): \(bases\) primary bases '(.*?)' and '(.*?)' are the same/) {
	my($file,$line,$base1,$base2) = ($1,$2,$3,$4);
	my $cfgw = find_cfgw($file,$line);
	my $pref = '';
	if ($cfgw) {
	    $pref = bases_prefer($cfgw,$base1,$base2) || '';
	} else {
	    warn "$file:$line: didn't find CFGW for line\n";
	    $pref = bases_prefer('',$base1,$base2) || '';
	}
	if ($pref eq $base1) {
	    warn "$file:$line: FYI: deleting base $base2 and keeping $pref\n";
	    delete_base($file,$line,$base2);
	} elsif ($pref eq $base2) {
	    warn "$file:$line: FYI: deleting base $base1 and keeping $pref\n";
	    delete_base($file,$line,$base1);
	} else {
	    # bases_prefer couldn't make a decision, so leave it
	    warn "$file:$line: bases_prefer failed to choose between $base1 and $base2 for word $cfgw\n";
	}

    } elsif (/^(.*?):(.*?): FORM .*? may not contain degree/) {
	my($file,$line) = ($1,$2);
	warn "exec form_degree_cdot\n";
	form_degree_cdot($file,$line);
    } else {
	
	if (/primary bases.*same/) {
	    warn "MISSED $_";
	} else {
	    warn "nothing to do with $_"
		unless /\(bases\)/ || /cbdpp/;
	}
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

sub delete_base {
    my($f,$l,$bad) = @_;
    my $ln = fix_get_line($f,$l);
    if ($ln =~ /^\@bases/) {
	my @bases = fix_bases_list($ln);
	my @nbases = ();
	my $bad_fixed = 0;
	foreach my $b (@bases) {
	    if ($$b[0] ne $bad) {
		push @nbases, $b;
	    } else {
		if ($bad_fixed++) {
		    push @nbases, $b;
		} else {
		    warn "deleting $$b[0]\n";
		}
	    }
	}
	if ($#nbases < $#bases) {
	    fix_set_line($l,fix_bases_back(@nbases));
	} else {
	    warn "$f:$l: $bad not found so not deleted\n";
	}
    } else {
	warn "$f:$l: not a \@bases line\n" if $verbose;
    }
}

sub form_degree_cdot {
    my ($file,$line) = @_;
    my $x = fix_get_line($file,$line);
    $x =~ s/form\s+(\S+)/_degree_cdot($1)/e;
    fix_set_line($line,$x);
}
sub _degree_cdot {
    my $t = shift;
    $t =~ tr/°·//d;
    "form $t";
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
    if ($ln =~ /^\@form/ || $ln =~ /\t\@form/) {
	my ($prebase,$base,$postbase) = $ln =~ m#^(.*?\@form\s+\S+\s+.*?)/(\S+)\s+(.*)\s*$#;
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

sub fix_in_form_form {
    my($f,$l,$bad,$good) = @_;
    warn "fix_in_form_form: bad=$bad; good=$good\n";
    my $ln = fix_get_line($f,$l);
    if ($ln =~ /^\@form/ || $ln =~ /\t\@form/) {
	my ($preform,$form,$postform) = $ln =~ m#^(.*?\@form\s+)(\S+)(\s+.*?)$#;
	my $orig_form = $form;
	my $badQ = quotemeta($bad);
	my $nfix = 0;
	my $n = 0;
	$good = fix_hide_bound($good);
	do {
	    $n = ($form =~ s#(^|$bound)$badQ($|$bound)#$1$good$2#);
	    ++$nfix if $n;
	} while ($n);
	$form = fix_show_bound($form);
	if ($nfix) {
	    warn "form $orig_form => $form\n";
	    fix_set_line($l,"$preform $form $postform");
	} else {
	    warn "$f:$l: $bad not found as FORM in form (form=$form)\n";
	}
    } else {
	warn "$f:$l: not a \@form line\n" if $verbose;
    }
}

sub find_cfgw {
    my($f,$l) = @_;
    open_and_load($f) unless $f eq $curr_file;
    # $lines[$l-1];
    while ($lines[$l-1] !~ /^\@entry/) {
	--$l;
    }
    if ($lines[$l-1] =~ /\@entry\s+(.*?)\s*$/) {
	return $1;
    } else {
	return undef;
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
    foreach my $bb (bb_uniq(@bb)) {
	my $bbb = "@$bb";
	$bbb =~ s/\s*$//;
	push @bbb, $bbb;
    }
    $ln .= join('; ', @bbb);
    $ln;
}

sub bb_uniq {
    my @bb = @_;
    my @nbb = ();
    my %seen = ();
    # NB: This loses alt bases from duplicates
    foreach $b (@bb) {
	push @nbb, $b unless $seen{$$b[0]}++;
    }
    @nbb;
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
