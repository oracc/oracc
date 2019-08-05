#!/usr/bin/perl
use warnings; use strict; use open ':utf8'; use utf8;
use Getopt::Long;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $dump_table = undef;
my %edits = ();
my %whens = ();
my $edit_pending = 0;

my $when = '';
my $from = '';
my $to = '';
my $table = '';

GetOptions (
    'dump:s'=>\$dump_table,
    'from:s'=>\$from,
    'table:s'=>\$table,
    'to:s'=>\$to,
    'when:s'=>\$when,
    );

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
    $edits{'e1'} = { when=>$when, 
		     from=>$from,
		     to=>$to,
		     done=>0 };
    foreach my $w (split(/,/,$when)) {
	push @{$whens{$w}}, 'e1';
    }
}

while (<>) {
    if (/^\s*$/ || /^[&\@\$]/ || /^[:=]/) {
	print unless $dump_table;
	next;
    } elsif (/^\#/) {
	print unless $dump_table;
	next;
    } else {
	print unless $dump_table; # get rid of the line before hacking it up
	my $line = $_;
	my $lem = <> || last;
	if ($lem =~ /^\#lem:/) {
	    chomp $line; chomp $lem;
	    $line =~ s/\(\#.*?\#\)//g;
	    $line =~ s/\(\$.*?\$\)//g;
	    $line =~ s/\{\{/ /g;
	    $line =~ s/\}\}/ /g;
	    $line =~ s/^\S*\s+//;
	    $line =~ s/\s$//;
	    $line =~ s/\%[a-z]+\s*//g;
	    $lem =~ s/^\S*\s+//;
	    $lem =~ s/\s$//;
	    $line =~ s/<<.*?>>//g;
	    $line =~ s/--/-/;
	    $line =~ tr/-:. a-zA-Z0-9šŋŠŊ₀-₉ₓ\|\@&~%{}()//cd;
	    $line =~ s/\s+/ /g;
	    $line =~ s/\(::\)//g; # for etcsl
	    $line =~ s/:\(MIN\)//g; # for liturgies
	    $line =~ s/:\?//g; # for liturgies

	    my @line = grep(defined&&length&&!/^%/&&!/^\d+::\d+/ , split(/\s+/, $line));
	    my @lem = grep(defined&&length, split(/;\s+/, $lem));
	    if ($#line != $#lem) {
		warn("$.: $#line != $#lem\n");
		warn("$.:tlt: ", join('|',@line),"\n");
		warn("$.:lem: ", join('|',@lem),"\n");
		print("#lem: ", join('; ', @lem), "\n") unless $dump_table;
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
				    $lem[$i] = $$ehash{'to'};
				    ++$$ehash{'done'};
				    last;
				}
			    } else {
				foreach my $l (split(/\|/,$lem[$i])) {
				    next unless $l;
				    if ($l eq $$ehash{'from'}) {
					$lem[$i] = $$ehash{'to'};
					++$$ehash{'done'};
					last;
				    }
				}
			    }
			}
		    }
		}
		print "#lem: ", join('; ', @lem), "\n"
		    unless $dump_table;
	    }
	} else {
	    print $lem unless $dump_table;
	}
    }
}

my $total = 0;
foreach my $e (keys %edits) {
    my $ehash = $edits{$e};
    $total += $$ehash{'done'};
    print STDERR
	"$$ehash{'done'}: when $$ehash{'when'} map $$ehash{'from'} to $$ehash{'to'}\n";
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
	die "$0:$.: bad syntax\n" unless $when && $from && $to;
	$edits{$e} = { when=>$when, 
			 from=>$from,
			 to=>$to,
			 done=>0 };
	push @{$whens{$when}}, $e;
	++$e;
    }
    close(T);
}

1;
