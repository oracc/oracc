#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
binmode STDOUT, ':utf8';
use Getopt::Long;
use Data::Dumper;
use constant {
    NON=>0,
    GLO=>1,
    XTF=>2,
    PRX=>3
    };

my $project = (`oraccopt`);
my $type = (`oraccopt . type`);
my $dynamic = (`oraccopt . cbd-mode` eq 'dynamic');

# This can't be right: set cbd-mode=dynamic if you want
# this behaviour
# $dynamic = 1 if $type eq 'superglo';

#warn "l2p1-project-sig.plx: dynamic=$dynamic\n";

my %langs = ();
my $lemm_only = 0;
my %project_sigs = ();
my %ranks = ();
my @sigs = ();
my %sigs = ();
my $state = NON;
my %totals = ();

GetOptions(
    'lemm-only'=>\$lemm_only,
    );

my @cand = ();

# This allows a superglo to have a glossary that constrains the umbrella
# children.  If there is no superglo glossary, from-glos.sig won't exist
# and everything from umbrellas will come into top level glossary
if (-r '01bld/from-glos.sig' && -s _) {
    push @cand, ('01bld/from-glos.sig');
} else {
    $dynamic = 1;
}

unless ($lemm_only) {
    push @cand, '01bld/from-xtf-glo.sig', '01bld/from-prx-glo.sig';
    push @cand, '01bld/from-xtf-new.sig' if $dynamic;
}

foreach my $c (@cand) {
    if (-r $c && -s _) {
	push @sigs, $c;
     }
}

# no concept of not-in-glo when cbd-mode is dynamic
unless ($dynamic) {
    open(XLOG, '>01bld/xtf-sigs-not-in-glo.log');
    open(PLOG, '>01bld/prx-sigs-not-in-glo.log');
}

foreach my $s (@sigs) {
    $state = set_state($s);
    open(S,$s);
    my $fields = <S>;
    while (<S>) {
	next if /^\s*$/;
	chomp;
	next if /\!0x(\d+)/;
	/\%(.*?):/;
	my $lang = $1;
	my $lang_no_script = $lang;
	$lang_no_script =~ s/-[0-9]*$//;
	++$langs{$lang_no_script};
	/:(.*?)=/;
	my $base = $1;
	s,^\@.*?%,\@$project%,;
	if ($fields =~ /inst/ && /^(.*?)\t(.*?)$/) {
	    my($sig,$inst) = ($1,$2);
	    if (defined($sigs{$sig}) || $dynamic) {
		my @refs = split(/\s/,$inst);
		@{$sigs{$sig}}{@refs} = ();
		${$totals{$lang_no_script}}{$base} += ($#refs + 1);
	    } else {
		if ($state == XTF) {
		    print XLOG "$sig\n";
		} elsif ($state == PRX) {
		    print PLOG "$sig\n";
		} else {
		    die "l2p1-project-sig.plx: this can't happen; processing sigs in unknown state\n";
		}
	    }
	} elsif ($fields =~ /rank/ && /^(.*?)\t(.*?)$/) {
	    my($sig,$rank) = ($1,$2);
	    %{$sigs{$1}} = ();
	    $ranks{$1} = $2;
	} else {
	    warn "$s:$.: ignoring bad .sig file line\n";
	}
    }
  SIGFILE_END:
    close(S);
}

unless ($dynamic) {
    close(XLOG);
    close(PLOG);
}

#foreach my $l (keys %langs) {
#    my $fh = undef;
#    open($fh, '>', "01tmp/rank-$l.sig");
#    print $fh "\@fields sig rank\n";
#    $langs{$l} = $fh;
#}

open(P, '>01bld/project.sig');
print P "\@fields sig rank freq inst\n";
foreach my $s (sort keys %sigs) {
    next if $s =~ /^\@field/;
    $s =~ /\%(.*?):/;
    my $this_lang = $1;
    $this_lang =~ s/-[0-9]+$//;
    $s =~ /:(.*?)=/;
    my $base = $1;
    my $ninst = scalar keys %{$sigs{$s}};
    my $total = ${$totals{$this_lang}}{$base};
    my $pct = ipct($ninst, $total);
    my $rank = $ranks{$s} || '0';
#    print {$langs{$this_lang}} "$s\t$rank\n";
    print P "$s\t$rank\t$ninst\t", join(' ', sort keys %{$sigs{$s}}), "\n";
}
close(P);

#foreach my $l (keys %langs) {
#    close($langs{$l});
#}

#############################################################

sub
ipct {
    my($amount,$total) = @_;
    if ($total) {
	my $pct = 100 * ($amount/$total);
	return int($pct+.5);
    } else {
	return 0;
    }
}

sub
set_state {
    if ($_[0] =~ /-glos\./) {
	return GLO;
    } elsif ($_[0] =~ /-xtf-/) {
	return XTF;
    } elsif ($_[0] =~ /-prx-/) {
	return PRX;
    } else {
	system 'touch', '01bld/cancel';
	die "l2p1-project-sig.plx: unknown sigs file `$_[0]'; REBUILD CANCELLED\n";
    }
}

1;
