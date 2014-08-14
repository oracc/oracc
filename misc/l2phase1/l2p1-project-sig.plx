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

$dynamic = 1 if $type eq 'superglo';

warn "l2p1-project-sig.plx: dynamic=$dynamic\n";

my %langs = ();
my $lemm_only = 0;
my %project_sigs = ();
my @sigs = ();
my %sigs = ();
my $state = NON;
my %totals = ();

GetOptions(
    'lemm-only'=>\$lemm_only,
    );

my @cand = ('01bld/from-glos.sig');

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
    while (<S>) {
	chomp;
	next if /\!0x(\d+)/;
	/\%(.*?):/;
	my $lang = $1;
	my $lang_no_script = $lang;
	$lang_no_script =~ s/-[0-9]*$//;
	++$langs{$lang_no_script};
#	/^.*?\%.*?:(.*?\].*?)'/;
	/:(.*?)=/;
	my $base = $1;
#	$base =~ s#//.*?]#]#;
	s/^\@.*?%/\@$project%/;
	if (/^(.*?)\t(.*?)$/) {
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
	} else {
	    %{$sigs{$_}} = ();
	}
    }
    close(S);
}

unless ($dynamic) {
    close(XLOG);
    close(PLOG);
}

#warn Dumper \%sigs;

foreach my $l (keys %langs) {
    my $fh = undef;
    open($fh, '>', "02pub/lemm-$l.sig");
    $langs{$l} = $fh;
}

open(P, '>01bld/project.sig');
foreach my $s (sort keys %sigs) {
    $s =~ /\%(.*?):/;
    my $this_lang = $1;
    $this_lang =~ s/-[0-9]+$//;
#    $s =~ /^.*?\%.*?:(.*?\].*?)'/;
    $s =~ /:(.*?)=/;
    my $base = $1;
#    $base =~ s#//.*?]#]#;
    my $ninst = scalar keys %{$sigs{$s}};
    my $total = ${$totals{$this_lang}}{$base};
    my $pct = ipct($ninst, $total);
    print {$langs{$this_lang}} "$s\t$ninst\t$pct\n";
    print P "$s\t$ninst\t", join(' ', sort keys %{$sigs{$s}}), "\n";
}
close(P);

foreach my $l (keys %langs) {
    close($langs{$l});
}

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
	die "l2p1-project-sig.plx: unknown sigs file `$_[0]'; stop\n";
    }
}

1;
