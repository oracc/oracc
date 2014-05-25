#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Builtins;

$ORACC::L2GLO::Builtins::bare = 1;
my $EMULT = 1000;

my $chatty = 1;
my %map = ();
my %sort = ();
my @warnings = ();

my @glo = <00lib/*.glo>;

die "super compare: a super-glossary is must have a .glo file\n" if $#glo < 0;
die "super compare: a super-glossary is only allowed one .glo file\n" if $#glo > 0;

my $base = shift @glo;
my $src = shift @ARGV;

die "super compare: unable to open base glossary $base. Stop.\n" unless -r $base;
die "super compare: unable to open source glossary $src. Stop.\n" unless -r $src;

chatty("super glossary comparison routine:");
chatty("using base glossary = $base");
chatty("using comparison glossary = $src");

my $basedata = ORACC::L2GLO::Builtins::input_acd($base);
my $srcdata = ORACC::L2GLO::Builtins::input_acd($src);

my %basehash = %{$$basedata{'ehash'}};
my %srchash = %{$$srcdata{'ehash'}};

add_basehash_senses();

my $map = $src; $map =~ s/glo$/map/;
load_map($map) if -r $map;

foreach my $e (keys %srchash) {
    my $mapped = 0;
    my %e = %${$srchash{$e}};
    my $eline = $e{'#line'} * $EMULT;
    $sort{$e} = $eline;
    unless (defined $basehash{$e}) {
	$map{$e} = [ 'new', 'entry', $e , '' ] unless $map{$e};
    } else {
	my %s = ();
	my %b = %${$basehash{$e}};
	@s{@{$b{'sense'}}} = ();
	foreach my $s (sort @{$e{'sense'}}) {
	    unless (exists $s{$s}) {
		my($epos,$sense) = ($s =~ /^(\S+)\s+(.*)$/);
		my $newsense = $e;
		$newsense =~ s#\]#//$sense]#;
		$newsense .= "'$epos";
		$sort{$newsense} = $eline;
		$map{$newsense} = [ 'new', 'sense', $newsense, '' ] unless $map{$newsense};
	    }
	}
    }
}

dump_map($map);

chatty("done.");

##################################################################################

sub
add_basehash_senses {
    foreach my $e (keys %basehash) {
	my %b = %${$basehash{$e}};
	foreach my $s (@{$b{'sense'}}) {
	    my($epos,$sense) = ($s =~ /^(\S+)\s+(.*)$/);
	    my $newsense = $e;
	    $newsense =~ s#\]#//$sense]#;
	    $newsense .= "'$epos";
	    $basehash{$newsense} = $basehash{$e};
	}
    }
}

sub
chatty {
    if ($chatty) {
	warn @_, "\n";
    }
}

sub
dump_map {
    my $m = shift;
    my @v = values %map;
    foreach my $v (@v) {
	warn "no sort code for $$v[2]\n" unless $sort{$$v[2]};
    }
    chatty("writing new map file $m");
    open(M, ">$m") || die "super compare: can't write to map file $m\n";
    select M;
    foreach my $w (sort { $sort{$$a[2]} <=> $sort{$$b[2]} } values %map) {
	print "$$w[0] $$w[1] $$w[2]";
	print " => $$w[3]" if $$w[3];
	print "\n";
    }
    close(M);
}

sub
load_map {
    my $m = shift;
    chatty("loading map file $m");
    open(M, $m) || die "super compare: can't read map file $m\n";
    while (<M>) {
	my($act,$type,$sig) = (/(\S+)\s+(\S+)\s+(.*?)$/);
	my $map = '';
	warn "$m:$.: bad map entry\n" and next unless $sig;
	next if $act eq 'new';
	if ($sig =~ /^(.*?)\s*=>\s*(.*?)$/) {
	    ($sig,$map) = ($1,$2);
	}
	if ($act eq 'map') {
	    warn "$m:$.: $map not in base glossary\n" unless $basehash{$map};
	}
	$map{$sig} = [ $act, $type, $sig, $map ];
    }
    close(M);
}

1;
