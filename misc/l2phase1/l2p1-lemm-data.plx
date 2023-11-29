#!/usr/bin/perl
use warnings; use strict; use open ':utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Getopt::Long;
use Data::Dumper;

#my $g2 = `oraccopt . g2`;
#$g2 = undef unless ($g2 && $g2 eq 'yes');

my $g2 = 1;

my $cbd_mode = `oraccopt . cbd-mode`;

my $lem_allow_x = `oraccopt . lem-allow-x`;
$lem_allow_x = undef unless ($lem_allow_x && $lem_allow_x eq 'yes');

my %f = ();
my $force = 0;
my $freq = 0;
my $glossary = '';
my %lemmdata = ();
my $lemmglo = '';
my $rank = 0;
my %sigorder = ();
my $update = 0;
my $verbose = 1;

GetOptions (
    force=>\$force, 
    'glossary'=>\$glossary,
    'lemmglo:s'=>\$lemmglo,
    'update'=>\$update
    );

#warn "l2p1-lemm-data.plx: running with -u flag for update\n" if $update;

my %freqs = ();
my %ranks = ();
my @sigs = ();

my @glosigs = ();

if ($glossary || $lemmglo) {
    my @glo = ();
    if ($lemmglo) {
	push @glo, $lemmglo;
    } else {
	push @glo, <00lib/*.glo>;
    }
    foreach my $g (@glo) {
	warn "$0: loading $g in glossary-only mode\n";
	push @glosigs, `cbdpp.plx -sig -std $g`;
	# open(G, '>glosig.dump'); print G Dumper \@glosigs; close(G);
    }
} else {
    my @glosig_files = ();
    if ($g2) {
	if ($cbd_mode eq 'dynamic') {
	    @glosig_files = `ls 01bld/*/union.sig`; chomp @glosig_files;
	    my @n = ();
	    foreach my $g (@glosig_files) {
		my $l = $g; $l =~ s#01bld/##; $l =~ s#/.*$##;
		system "$ENV{'ORACC_BUILDS'}/bin/l2p1-union-ranks.plx", $l;
		push @n, "01bld/$l/union-ranks.sig";
	    }
	    @glosig_files = @n;
	} else {
	    @glosig_files = `ls 01bld/*/from_glo.sig`;
	}
	chomp @glosig_files; @glosig_files = grep /\.sig$/, @glosig_files;
    } else {
	@glosig_files = ('01bld/from-glos.sig');
    }
    die "l2p1-lemm-data.plx: no glossary signature files. Stop.\n"
	unless $#glosig_files >= 0;
    foreach my $f (@glosig_files) {
	die "l2p1-lemm-data.plx: no signatures file $f\n"
	    unless (open(F, $f));
	warn "reading glossary sigs from $f ...\n";
	push @glosigs, (<F>);
	close(F);
    }
}
chomp @glosigs;
foreach (@glosigs) {
    next if /^\@(?:project|name|lang)/ || /^\s*$/ || /\!0x0/; # ignore COFs in the 0x0 format because they used in the && format
    if (/^\@fields/) {
	set_f($_, qw/sig rank/);
	next;
    }
    my @fields = split(/\t/,$_);
    push @sigs, $fields[0];
    $ranks{$fields[0]} = $fields[$rank];
}

# # Harvest sigs and ranks from the project's glossaries
# foreach my $f (@glosig_files) {
#     die "l2p1-lemm-data.plx: no signatures file $f\n"
# 	unless (open(F, $f));
#     warn "reading glossary sigs from $f ...\n";
#     while (<F>) {
# 	next if /^\@(?:project|name|lang)/ || /^\s*$/ || /\!0x0/; # ignore COFs in the 0x0 format
# 	chomp;
# 	if (/^\@fields/) {
# 	    set_f($_, qw/sig rank/);
# 	    next;
# 	}
# 	my @fields = split(/\t/,$_);
# 	push @sigs, $fields[0];
# 	$ranks{$fields[0]} = $fields[$rank];
#     }
#     close(F);
# }

my @freq_files = ();
if ($update) {
    @freq_files = <02pub/lemm-*.sig>;
} else {
    @freq_files = ('01bld/project.sig');
}

# Harvest freqs from the project.sig or the last set of lemm-data
foreach my $l (@freq_files) {
    if (open(L,$l)) {
	warn "$0: harvesting sig frequencies from $l ...\n";    
	while (<L>) {
	    next if /^\s*$/ || m/^\@(?:project|name|lang)/;
	    chomp;
	    if (/^\@fields/) {
		set_f($_, qw/sig freq/);
		next;
	    }
	    my @fields = split(/\t/,$_);
	    $freqs{$fields[0]} = $fields[$freq];
	}
	close(L);
    } else {
	die "l2p1-lemm-data.plx: can't open `$l' for update\n"
	    unless $force;
    }
}

# open(D,'>freq.dump'); print D Dumper \%freqs; close(D);

# Add freqs to sigs/ranks and dump new lemm-data. If a project
# is using dynamic lemm-data from proxies that proxy data will 
# not make it into this version. This is a limitation of update.
my $sigorder = 0;
foreach my $s (@sigs) {
    my ($form) = ($s =~ /:(.*?)=/);
    if ($form eq 'x' && !$lem_allow_x) {
	warn "$0: form 'x' illegal in lemm data ($s)\n";
	next;
    }
    if ($s =~ s/\@(?:EG|ES|UGN)//) {
#	warn "removed EG/ES/UGN\n";
    }
    my ($r,$f) = (0,0);
    $r = $ranks{$s} if $ranks{$s};
    $f = $freqs{$s} if $freqs{$s};
    my $lng = lng_of($s);
    warn "no lng in $s\n" unless $lng;
    $lng =~ s/-\d\d\d//; # remove script codes
    $sigorder{$s} = ++$sigorder;
    $s = "$s\t$r\t$f\n";
    push @{$lemmdata{$lng}}, $s;
}

my %use_psus = ();
my $use_psus = `oraccopt . cbd-use-psus`;
if ($use_psus) {
    foreach my $u (split(/\s+/,$use_psus)) {
	my($l,$p) = ($u =~ /^\%(.*?):(.*?)$/);
	if ($l && $p) {
	    $use_psus{$l} = $p;
	} else {
	    warn "$0: bad spec in cbd-use-psus: $u should be '\%LANG:PROJ'\n";
	}
    }
}

foreach my $l (keys %lemmdata) {
    open(L, ">02pub/lemm-$l.sig") 
	|| die "l2p1-lemm-data.plx: Strange, can't write 02pub/lemm-$l.sig. Stop";
    print L "\@fields sig rank freq\n";
    print L sort { &byfreq }  uniq(@{$lemmdata{$l}});
    if ($use_psus{$l}) {
	my $psu_lemm_sig = "$ENV{'ORACC_BUILDS'}/$use_psus{$l}/02pub/lemm-$l.sig";
	if (-r $psu_lemm_sig) {
	    warn "$0: using PSUs from $psu_lemm_sig\n";
	    print L `grep '^{' $psu_lemm_sig`;
	} else {
	    warn "$0: can't find $psu_lemm_sig to use PSUs from\n";
	}
    }    
    close L;
}

####################################################################################

sub lng_of {
    if ($_[0] =~ /^\{/) {
	$_[0] =~ /\}::.*?\%(.*?):/;
	return $1;
    } else {
	$_[0] =~ /\%(.*?):/;
	return $1;
    }
}

sub byfreq {
    my ($af) = ($a =~ /(\d+)$/);
    my ($bf) = ($b =~ /(\d+)$/);
    if ($bf || $af) {
	$bf <=> $af;
    } else {
	my ($as) = ($a =~ /^(.*?)\t/);
	my ($bs) = ($b =~ /^(.*?)\t/);
	$sigorder{$as} = 0 unless $sigorder{$as};
	$sigorder{$bs} = 0 unless $sigorder{$bs};
	$sigorder{$as} <=> $sigorder{$bs};
    }
}

sub set_f {
    my ($fields,@required) = @_;
    %f = ();
    my @f = split(/\s/, $fields);
    shift @f; # drop '@field';
    for (my $i = 0; $i <= $#f; ++$i) {
	$f{$f[$i]} = $i;
    }

    if ($#required >= 0) {
	foreach my $rq (@required) {
	    die "l2p1-lemm-data.plx: can't rebuild  without all of @required. Stop\n"
		unless exists $f{$rq};
	}
    }
 
    $rank = $f{'rank'};
    $freq = $f{'freq'};
}

sub uniq {
    my %x = ();
    @x{@_} = ();
    keys %x;
}

1;
