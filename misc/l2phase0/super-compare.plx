#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2P0::L2Super;
use ORACC::L2GLO::Util;

$ORACC::L2GLO::Builtins::bare = 1;
my $EMULT = 1000;

my %map = ();
my %sort = ();
my @warnings = ();

my %data = ORACC::L2P0::L2Super::init();
close($data{'input_fh'}); # we're going to use this via input_acd
my $base = $data{'base'};
my $src = $data{'input'};

chatty("super glossary comparison routine:");
chatty("using base glossary = $base");
chatty("using comparison glossary = $src");

my $basedata = ORACC::L2GLO::Builtins::input_acd($base);
my %basehash = %{$$basedata{'ehash'}};
add_basehash_senses();

my %srchash = %{$data{'srchash_ref'}};

# This can be undefined if the map file doesn't exist yet
if ($data{'mapref'}) {
    %map = %{$data{'mapgloref'}};
}

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

dump_map();

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
dump_map {
    my @v = values %map;
    foreach my $v (@v) {
	warn "@$v\n";
	warn "no sort code for $$v[2]\n" unless $sort{$$v[2]};
    }
    chatty("writing new map file $data{'output'}");
    my $mapfh = $data{'output_fh'};
    select $mapfh;
    foreach my $w (sort { $sort{$$a[2]} <=> $sort{$$b[2]} } values %map) {
	print "$$w[0] $$w[1] $$w[2]";
	print " => $$w[3]" if $$w[3];
	print "\n";
    }
    close($mapfh);
}

1;
