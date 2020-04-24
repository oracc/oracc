#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use Getopt::Long;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $all = 1;
my $first = 1;
my %header = ();
my $lang = '';
my $quiet = 0;
my $project = '';
my %sig = ();
my $superglo = 0;
my $verbose = 0;

GetOptions(
    'lang:s'=>\$lang,
    'project:s'=>\$project,
    'quiet'=>\$quiet,
    'super'=>\$superglo,
    );

## No, we have to move this downstream to prevent failed resolution of parts of PSUs
#my $withall = `oraccopt . cbd-with-all`;
#$all = 1 if $withall && $withall eq 'yes';

my %f = ();

if ($superglo) {
    unshift @ARGV, "01bld/$lang/from_glo.sig";
}
open(NOTF, '>01tmp/sigs-not-in-superglo.tab');
foreach my $s (@ARGV) {
    unless (open(S,$s)) {
	warn "l2-sig-union.plx: no sig file $s\n"
	    unless $quiet;
	next;
    }
    $f{'inst'} = 1; # default for .sig files
    warn "l2-sig-union.plx: processing $s\n" if $verbose;
    while (<S>) {
	chomp;
	next if /^\s*$/;
	if (/^\@(project|name|lang|fields)\s+(.*?)$/) {
	    if (/fields/) {
		my @f = split(/\s+/, $_); shift @f;
		%f = ();
		for (my $i = 0; $i <= $#f; ++$i) {
		    $f{$f[$i]} = $i;
		}
		next;
	    } else {
		$header{$1} = $2;
		if ($1 eq 'lang') {
		    $lang = $2;
		}
		next;
	    }
	}
	    
	my @t = split(/\t/, $_);
#	my $r = (($#t == 2) ? $t[2] : (($#t == 1) ? $t[1] : ''));
	my $r = ($f{'inst'} ? $t[$f{'inst'}] : '');
	my $sig = $t[0];
	# we have to map subproject project names to match PSUs
	if ($sig =~ /^\{/ && !$first) {
	    $sig =~ s/::\@.*?\%/::\@$project\%/;
	    $sig =~ s/\+\+\@.*?\%/++\@$project\%/g;
	}
	if ($superglo) {
	    # in a superglo we read the glossary sigs first, then only allow in
	    # the ones that have been vetted into the main superglo file
	    if ($first || $sig{$sig}) {
		my @r = split(/\s/, $r||'');
		if ($all || $#r >= 0) {
		    @{$sig{$sig}}{@r} = ();
		}
	    } else {
		#		print NOTF "$_\n";
		my $p = $r; $p =~ s/:.*$//; $p = '#NOREFS' unless $p;
		if ($lang =~ /^qpn/) {
		    print NOTF "$p\t$sig\t$r\n"
			if $t[0] =~ /\][A-Z]N/;
		} else {
		    print NOTF "$p\t$sig\t$r\n"
			unless $t[0] =~ /\][A-Z]N/;
		}
	    }
	} else {
	    my @r = split(/\s/, $r||'');
	    if ($all || $#r >= 0) {
		@{$sig{$t[0]}}{@r} = ();
	    }
	}
    }
    close(S);
    $first = 0;
}

close(NOTF);

$header{'project'} = $project unless $header{'project'};
$header{'lang'} = $lang unless $header{'lang'};
$header{'name'} = "$project $lang" unless $header{'name'};

foreach my $k (qw/project name lang/) {
    print "\@$k $header{$k}\n";
}
print "\n\@fields sig freq inst\n";

#use Data::Dumper; open(X,">dump.log");
#print X Dumper \%sig;
#close(X);

foreach my $s (sort keys %sig) {
    $s =~ s/\s*$//;
    print "$s\t";
    my @r = uniq(keys %{$sig{$s}});
    if ($#r >= 0) {
	printf "%d\t@r", $#r+1;
    } else {
	print "0";
    }
    print "\n";
}

sub
uniq {
    my %x = ();
    foreach (grep($_, @_)) {
	@x{grep($_, split(/\s+/,$_))} = ();
    }
    sort keys %x;
}

1;
