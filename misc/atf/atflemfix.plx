#!/usr/bin/perl
use warnings; use strict; use open ':utf8'; use utf8;
use Getopt::Long;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my %from= ();
my %from_nopos = ();

my $from = '';
my $to = '';
my $table = '';

GetOptions (
    'from:s'=>\$from,
    'table:s'=>\$table,
    'to:s'=>\$to,
    );

if ($table) {
    fail("no such table file '$table'") unless -r $table;
    load_table($table);
} else {
    unless ($from && $to) {
	fail("must use -table or both of -from/-to");
    }
    $from{$from} = $to;
}

system 'mkdir', '-p', 'new'; # we will just fail on file open if this doesn't work

foreach my $a (@ARGV) {
    if (open(A, $a)) {
	if (open(N, ">new/$a")) {
	    warn "atflemfix.plx: reading $a and writing new/$a\n";
	    while (<A>) {
		if (s/^\#lem:\s*//) {
		    chomp;
		    my @l = split(/;\s+/, $_);
		    my @nl = ();
		    foreach my $l (@l) {
			push @nl, fix($l);
		    }
		    chomp @nl;
		    print N "#lem: ", join('; ', @nl), "\n";
		} else {
		    print N;
		}
	    }
	} else {
	    warn "atflemfix.plx: unable to open new/$a for output\n";
	}
    } else {
	warn "atflemfix.plx: unable to open $a for input\n";
    }
}

sub fix {
    my $x = $_[0];
    $x =~ s/\+//;
    if ($from{$x}) {
	return $from{$x};
    } else {
	my ($tmp,$post) = ($x =~ m/^(.*\])(.*)$/);
	$post = '' unless $post;
	if ($from_nopos{$tmp}) {
	    warn "matched nopos\n";
	    $tmp = $from{${$from_nopos{$tmp}}[0]};
	    $tmp .= $post;
	    return $tmp;
	}
    }
    return $x;
}

sub bad {
    print STDERR 'atflemfix.plx: ', join('', @_), "\n";
}

sub fail {
    bad(@_);
    die "atflemfix.plx: failed\n";
}

sub load_table {
    my $t = shift;
    open(T,$t) || die;
    my $e = 'e0000';
    while (<T>) {
	next if /^\#/ || /^\s*$/;
	my($from,$to) = (/^(.*?)\t+(.*)$/);
	warn "$t:$.: syntax error, line must be FROM<TAB>TO\n" unless $from && $to;
	if ($from{$from}) {
	    warn "$t:$.: duplicate 'from' entry in table\n";
	} else {
	    $from =~ s/\s*\[(.*?)\]\s*/[$1]/;
	    $to =~ s/\s*\[(.*?)\]\s*/[$1]/;
	    ${from{$from}} = $to;
	    my $nopos = $from;
	    $nopos =~ s/\].*$/]/;
	    push @{$from_nopos{$nopos}}, $from;
	    ++$e;
	}
    }
    close(T);
}

1;
