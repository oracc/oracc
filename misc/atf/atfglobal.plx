#!/usr/bin/perl
use warnings; use strict; use open ':utf8'; use utf8;
use Getopt::Long;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;
use Encode;

my $dump_table = undef;
my %edits = ();
my %whens = ();
my $edit_pending = 0;

my $when = '';
my $from = '';
my $to = '';
my $table = '';
my $nofixes = 0;
my $reportall = 0;
my $xfix = 0; # fix X lemmatizations as well

GetOptions (
    'dump:s'=>\$dump_table,
    'from:s'=>\$from,
    nofixes=>\$nofixes,
    reportall=>\$reportall, 
    'table:s'=>\$table,
    'to:s'=>\$to,
    'when:s'=>\$when,
    'x'=>\$xfix
    );

open(L,'>atfglobal.log') || fail("unable to write atfglobal.log");

my @f = @ARGV;
fail("must give file(s) to process on command line") unless $#f >= 0;

if ($table) {
    fail("no such table file '$table'") unless -r $table;
    load_table($table);
} elsif ($dump_table) {
    open(DUMPTAB,">$dump_table") || 
	fail("can't write to dump file '$dump_table'");
} else {
    unless ($when && $from && $to) {
	fail("must use -table or all of -when/-from/-to");
    }
    Encode::_utf8_on($when);
    Encode::_utf8_on($from);
    Encode::_utf8_on($to);
    $edits{'e1'} = { when=>$when, 
		     from=>$from,
		     to=>$to,
		     done=>0 };
    foreach my $w (split(/,/,$when)) {
	push @{$whens{$w}}, 'e1';
    }
}

system 'mkdir', '-p', 'new';
foreach my $f (@f) {
    my $errfile = '';
    my $errline = '';
    open(F,$f) || fail("unable to open input $f");
    $errfile = $f;
    my $outfile = `basename $f`;
    $outfile = "new/$outfile";
    open(O,">$outfile") || fail("unable to open output $outfile");
    print STDERR "writing fixed $f to $outfile\n";
    while (<F>) {
	if (/^\s*$/ || /^[&\@\$]/ || /^[:=]/) {
	    print O unless $dump_table;
	    next;
	} elsif (/^\#/) {
	    print O unless $dump_table;
	    next;
	} else {
	    print O unless $dump_table; # get rid of the line before hacking it up
	    my $line = $_;
	    my $lem = <F> || last;
	    $errline = $.;
	    if ($lem =~ /^\#lem:/) {
		chomp $line; chomp $lem;
		$line =~ s/,!\S+//g; # remove field designators like ,!yn
		1 while $line =~ s/\s\&\d*\s/ /; # remove column seps
		$line =~ tr/[]⸢⸣//d;
		$line =~ s/\*\(AŠ\@c\)[#*!?]*\s+//;
		$line =~ s/\(\#.*?\#\)//g;
		$line =~ s/\(\$.*?\$\)//g;
		$line =~ s/\{\{/ /g;
		$line =~ s/\}\}/ /g;
		$line =~ s/^\S*\s+//;
		$line =~ s/\s$//;
		$line =~ s/\%[a-z-]+\s*//g;
		$lem =~ s/^\S*\s+//;
		$lem =~ s/\s$//;
		$line =~ s/<<.*?>>//g;
		$line =~ s/--/-/;
		$line =~ tr/:/-/;
		$line =~ tr/-:. a-zA-Z0-9šṣṭŋŠṢṬŊ₀-₉ₓ\|\@&~%{}()//cd;
		$line =~ s/\s+/ /g;
		$line =~ s/\(\d*::\d*\)//g; # for etcsl
		$line =~ s/:\(MIN\)//g; # for liturgies
		$line =~ s/:\?//g; # for liturgies
		
		my @line = grep(defined&&length&&!/^%/&&!/^\d+::\d+/ , split(/\s+/, $line));
		my @lem = grep(defined&&length, split(/;\s+/, $lem));
		if ($#line != $#lem) {
		    warn("$f:$.: $#line != $#lem\n");
		    warn("$.:tlt: ", join('|',@line),"\n");
		    warn("$.:lem: ", join('|',@lem),"\n");
		    print O "#lem: ", join('; ', @lem), "\n"
			unless $dump_table;
		} else {
		    for (my $i = 0; $i <= $#line; ++$i) {
			if ($dump_table) {
			    print DUMPTAB "$line[$i]\t$lem[$i]\n";
			} else {
			    my $w = $line[$i];
			    next unless $whens{$w};
			    foreach my $e (@{$whens{$w}}) {
				my $ehash = $edits{$e};
				if ($$ehash{'from'} =~ /\|/) {
				    if ($lem[$i] eq $$ehash{'from'}) {
					$lem[$i] = $$ehash{'to'} unless $nofixes;
					++$$ehash{'done'};
					print L "$errfile:$errline: when $w from $$ehash{'from'} to $$ehash{'to'}\n";
					last;
				    }
				} else {
				    foreach my $l (split(/\|/,$lem[$i])) {
					next unless $l;
					if ($l eq $$ehash{'from'} || ($xfix && $l eq 'X')) {
					    print L "$errfile:$errline: when $w from $$ehash{'from'} to $$ehash{'to'}\n";
					    $lem[$i] = $$ehash{'to'} unless $nofixes;
					    ++$$ehash{'done'};
					    last;
					}
				    }
				}
			    }
			}
		    }
		    print O "#lem: ", join('; ', @lem), "\n"
			unless $dump_table;
		}
	    } else {
		print O $lem unless $dump_table;
	    }
	}
    }
    close(F);
}

my $total = 0;
foreach my $e (keys %edits) {
    my $ehash = $edits{$e};
    $total += $$ehash{'done'};
    print STDERR
	"$$ehash{'done'}: when $$ehash{'when'} map $$ehash{'from'} to $$ehash{'to'}\n"
	if $reportall || $$ehash{'done'};
}
print STDERR "Total fixes: $total\n";

#######################################################

sub
bad {
    print STDERR 'atfglobal.plx: ', join('', @_), "\n";
}

sub
fail {
    bad(@_);
    die "atfglobal.plx: failed\n";
}

sub
load_table {
    my $t = shift;
    open(T,$t) || die;
    my $e = 'e0000';
    while (<T>) {
	next if /^\#/ || /^\s*$/;
	my($when,$from,$to) = (/^(\S+)\t+(.*?)\t+(.*)$/);
	die "$t:$.: bad syntax\n" unless $when && $from && $to;
	$edits{$e} = { when=>$when, 
			 from=>$from,
			 to=>$to,
			 done=>0 };
	push @{$whens{$when}}, $e;
	++$e;
    }
    close(T);
    # use Data::Dumper; print Dumper \%whens; exit 1;
}

close(L);

1;
