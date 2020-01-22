#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use ORACC::CBD::Bases;

my $input = shift @ARGV;

my $abbrev_flag = 0;
my $curr_cfgw = '';
my $cfgw_spaced = '';

my %baseconts = ();
my %count = ();
my %count_w_cont = ();
my @fit_counts = ();
my @fit_zeroes = ();
my $fit_max = 0;
my %insts = ();

my $nwords = 0;
#my @bcount = `base-count.plx 01bld/from-prx-glo.sig`; chomp(@bcount);
open(B,'00etc/base-count.tab');
while (<B>) {
    chomp;
    my($k,$c,$i) = split(/\t/,$_);
    $count_w_cont{$k} = $c;
    my($ba,$co) = ($k =~ /^(.*?)\+(?:(o)|-(.*))$/);
    $count{$ba} += $c;
    push @{$insts{$ba}}, $i;
    push @{$baseconts{$ba}}, $co;
}
close(B);

my %redup = (); load_redup();

open(SLOG,'>01tmp/soundex.log');
open(L,'>00etc/base-audit.tab'); select L;

bases_init();
open(IN, $input) || die "$0: can't open $input for base audit\n";
print "=file: $input\n";
while (<IN>) {
    if (/^[-+>]?\@(entry|bases)/) {
	if (/^[-+>]?\@entry\!?\s+(.*?)\s*$/) {
	    $cfgw_spaced = $curr_cfgw = $1;
	    $curr_cfgw =~ s/\s+\[/[/; $curr_cfgw =~ s/\]\s+/]/;
	} else {
	    s/^\@bases\s+//;
	    my %b = bases_hash($_);
	    audit(grep(!/\#/, sort keys %b)) if scalar keys %b;
	}
    }
}
bases_term();

summary();

warn "$0: audit results written to 00etc/base-audit.tab\n";

close(L);
close(SLOG);

#############################################################################

sub audit {
    my @b = @_;
#    warn "audit @b\n";
    my $cf = $curr_cfgw;    
    
    $cf =~ s/\[.*$//;
    $cf = mangle($cf);
    return if $cf =~ /\s/;

    my $l = $.+1;
    print "=entry:$l: $curr_cfgw: $cf ~ @b\n";

    my @i = ();
    my $total = 0;
    foreach my $b (@b) {
	my $k = "$curr_cfgw/\%sux:$b";
	my %info = ();
	$info{'base'} = $b;
	if ($count{$k}) {
	    $info{'count'} = $count{$k};
	    @{$info{'insts'}} = @{$insts{$k}};
	} else {
	    next;
	    $info{'count'} = 0;
	    @{$info{'insts'}} = ();
	}
	$total += $info{'count'};
	@info{qw/form fit/} = base_fit($cf,$b);

	if ($abbrev_flag) {
	    $info{'type'} = 'abb';
	    $abbrev_flag = 0;
	}

	if ($info{'fit'} == 9) {
	    warn "$input:$.: base $b does not fit $curr_cfgw\n";
	}
	
	++$fit_counts[$info{'fit'}];
	++$fit_zeroes[$info{'fit'}] unless $info{'count'};
	
	$fit_max = $info{'fit'} if $info{'fit'} > $fit_max && $info{'fit'} < 9;
	$info{'type'} = base_type($cf,$b) unless $info{'type'};
	push @i, { %info };
    }
    foreach my $i (sort { $$b{'count'} <=> $$a{'count'} } @i) {
	print_info($cf, %$i);
    }

    ++$nwords;
}

sub soundex1 {
    my $x = shift;
    $x =~ tr/bdgqziš/ptkksesm/;
    return $x;
}

sub soundex2 {
    my $x = shift;
    $x =~ tr/pnr/kll/;
    return $x;
}

sub soundex3 {
    my $x = shift;
    $x =~ tr/aeiu/vvvv/;
    1 while $x =~ s/vv/v/;
    $x;
}

sub summary {
    print "=summary\n";
    for (my $i = 0; $i <= $fit_max; ++$i) {
	if (defined $fit_counts[$i]) {
	    $fit_zeroes[$i] = 0 unless $fit_zeroes[$i];
	    print "$fit_counts[$i] fits at level $i\n"; # \[$fit_zeroes[$i] unattested]\n";
	}
    }
    print "$fit_counts[9] fit failures\n"; # \[$fit_zeroes[9] unattested]\n";
}

sub mangle {
    my $b = shift;
    $b =~ s/ₓ\(\|.*?\|[^(]*\)//;
    $b =~ s/ₓ\(.*$?\)//;
    $b =~ tr/-.+&%@₀-₉ʾ|//d;
    $b =~ s/ₓ\(.*?\)//g;
    $b =~ s/\{.*?\}//g;
    $b =~ s/^.*?°//;
    $b =~ s/·.*$//;
    1 while $b =~ s/([a-zA-ZŋŊšŠ])\1/$1/;
    $b =~ s/\(.*?\)$//;
    $b;
}

sub print_info {
    my($b, %i) = @_;
    print "$b";
    foreach my $k (qw/base form fit type count/) {
	if ($k eq 'fit') {
	    print "\t\@$i{$k}";
	} else {
	    print "\t$i{$k}";
	}
    }
    if ($i{'fit'} > 1) {
	print "\t@{$i{'insts'}}";
    }
    print "\n";
}

sub base_fit {
    my($c,$b) = @_;

    my $u = $b;
    $u =~ tr/A-ZŠŊ₀-₀ₓ|&()+.//d;
    return ($b,0) unless $u;
    
    my $fit = 1;

    return ($b,$fit) if ${$redup{$cfgw_spaced}}{$b};

    $fit = 2;
    $b = mangle($b);
    1 while $b =~ s/([a-z])\1/$1/;

    return ($b,$fit) if $b eq $c;

    $fit = 3;
    my $bq = quotemeta($b);
    if ($c =~ /^$bq(.+)$/) {
	my $rest = $1;
	$abbrev_flag = 1 if length($rest) > 1;
	return($b,$fit);	    
    }
    
    $fit = 4;
    if ($c =~ /k$/) {
	my $tmp = $c;
	$tmp =~ s/k$//;
	return ($b,$fit) if $b eq $tmp;
    }

    $fit = 5;
    my $stemmed_c = stem($c);
    my $stemmed_b = stem($b);

    if (close_enough($stemmed_c, $stemmed_b)) {
	print SLOG "$c > $stemmed_c === $b > $stemmed_b\n";
	return ($b,$fit);
    } else {
	print SLOG "$c > $stemmed_c !== $b > $stemmed_b\n";	
    }

    $fit = 6;
    my $soundex_c = soundex1($c);
    my $soundex_b = soundex1($b);

    if (close_enough($soundex_c,$soundex_b)) {
	print SLOG "$c > $soundex_c == $b > $soundex_b\n";
	return ($b,$fit);
    } else {
	$soundex_c = soundex2($soundex_c);
	$soundex_b = soundex2($soundex_b);
	if (close_enough($soundex_c, $soundex_b)) {
	    print SLOG "$c > $soundex_c == $b > $soundex_b\n";
	    return ($b,$fit);
	} else {
	    $soundex_c = soundex3($soundex_c);
	    $soundex_b = soundex3($soundex_b);
	    if (close_enough($soundex_c, $soundex_b)) {
		print SLOG "$c > $soundex_c == $b > $soundex_b\n";
		return ($b,$fit);
	    } else {
		my ($c1,$c2) = ($soundex_c,$soundex_b);
		$c1 =~ tr/v//d; $c2 =~ tr/v//d;
		if ($c1 eq $c2) { # must be exact match
		    print SLOG "$c > $c1 == $b > $c2\n";
		    return ($b,$fit);
		} else {
		    print SLOG "$c > $soundex_c != $b > $soundex_b\n";
		}
	    }
	}
    }

    $fit = 7;
    $stemmed_c = stem($soundex_c);
    $stemmed_b = stem($soundex_b);

    if (close_enough($stemmed_c, $stemmed_b)) {
	print SLOG "$c > $stemmed_c === $b > $stemmed_b\n";
	return ($b,$fit);
    } else {
	print SLOG "$c > $stemmed_c !== $b > $stemmed_b\n";	
    }

    $fit = 8;
    if (redup($c,$b)) {
	return ($b,$fit);
    }
    
    $fit = 9;
    ($b,$fit);
}

sub base_type {
    'log';
}

sub close_enough {
    my($a,$b) = @_;
#    warn "close_enough $a <=> $b entry\n";
    return 1 if $a eq $b;
    my $aq = quotemeta($a);
    return 1 if $b =~ /^$aq.$/ || $b =~ /^.$aq$/;
    my $bq = quotemeta($b);
    return 1 if $a =~ /^$bq.$/ || $a =~ /^.$bq$/;
#    warn "close_enough $a <=> $b failed\n";
    return 0;
}

sub redup {
    my($c,$b) = @_;
    $b =~ /^($c)\1/;
}

sub stem {
    my $x = shift;
    $x =~ s/^[aeiu](.)/$1/;
    $x =~ s/(.)(?:a|ak|e|i|um|u)$/$1/;
    $x;
}

sub load_redup {
    if (-r '00etc/base-redup.tab') {
	if (open(R,'00etc/base-redup.tab')) {
	    while (<R>) {
		chomp;
		my($cfgw,$data) = split(/\t/, $_);
		$data =~ (/^(\S+)\s/);
		++${$redup{$cfgw}}{$1};
	    }
	    close(R);
	} else {
	    warn "$0: can't read 00etc/base-redup.tab; proceeding without reduplication data\n";
	}
    } else {
	warn "$0: can't find 00etc/base-redup.tab; proceeding without reduplication data\n";
    }

}

1;
