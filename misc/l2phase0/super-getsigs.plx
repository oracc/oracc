#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2P0::L2Super;
use ORACC::L2GLO::Util;

$ORACC::L2P0::L2Super::chatty = 0;

#
# Retrieve sigs from from-xtf-glo.sig because that gives only the instances
# that are in a project's edited texts: if we use project.sig we can get
# things from proxies which would create duplication with superglossaries.
#

my %data = ORACC::L2P0::L2Super::init();

my %mapcut = %{$data{'map_cut'}};
my %mapglo = %{$data{'map_glo'}};
my %mapsigs = ();

my $mapproj = $data{'project'};
my $maplang = $data{'lang'};
my $outfh = $data{'output_fh'};

$mapproj =~ tr,-,/,;
my $imports = '';
my $projsigs = "$ENV{'ORACC'}/bld/$mapproj/from-xtf-glo.sig";
my $union = "$ENV{'ORACC'}/bld/$mapproj/$maplang/union.sig";

if (-r $projsigs) {
    super_die("super-getsigs.plx: can't read `$projsigs'. Stop.\n")
	unless open(S, $projsigs);
    $imports = $projsigs;
} else {
    if (-r $union) {
	super_die("super-getsigs.plx: can't read `$union'. Stop.\n")
	    unless open(S, $union);
	$imports = $union;
    } else {
	super_die("super-getsigs.plx: can't proceed without `$projsigs' or `$union'. Stop.\n");
    }
}

my $projdate = (stat($imports))[9];

my $mapdate = (stat($data{'mapfile'}))[9];

# if 
#   a) the map file is newer than the sigfile we must rebuild
# or
#   b) the sigfile is older than the project sigfile we must rebuild

if (!$data{'force'} && defined $data{'outputdate'}) {
    if ($mapdate < $data{'outputdate'} && $projdate < $data{'outputdate'}) {
	super_warn("$data{'output'} is up to date");
	exit 0;
    }
}

$outfh = undef;
open($outfh, '>', $data{'output'}) 
    || die "super-getsigs.plx: unable to open $data{'output'} to save sigs from $imports\n";

super_warn("importing sigs from $imports");

### ADD CHECK FOR REFERENCED/REDUNDANT map/fix ENTRIES
my %f = ();

while (<S>) {
    next if /^\@(project|name|lang)\s/ || /^\s*$/;
    chomp;
    if (s/^\@fields\s+//) {
	my @f = split(/\s+/, $_);
	for (my $i = 0; $i <= $#f; ++$i) {
	    $f{$f[$i]} = $i;
	}
    } elsif (!defined($f{'inst'})) {
	my @tmp = split(/\t/,$_);
	if ($tmp[$#tmp] =~ /:[PQX]/) {
	    $f{'inst'} = $#tmp;
	} else {
	    warn "no instances found in sigfile $imports\n";
	    last;
	}
    }
    my @s = split(/\t/,$_);
    my($sig,$inst) = ($s[0],$s[$f{'inst'}]);
    next unless $inst;

    if ($ORACC::L2P0::L2Super::qpn_lang) {
	next unless /\%$ORACC::L2P0::L2Super::qpn_lang/;
	next unless /\]([A-Z]N)/; # superglo only takes words with primary POS=NN
    } else {
	# next unless /\%$data{'baselang'}/; # not good enough because NN in sux.sig have %sux lang
	next if /\][A-Z]N'/; # skip entries with POS=NN
    }

    # For now, just deal with entry/sense level mapping
    my %sig = parse_sig($sig);
    my $entry = "$sig{'cf'}\[$sig{'gw'}\]$sig{'pos'}";
    my $sense = "$sig{'cf'}\[$sig{'gw'}//$sig{'sense'}\]$sig{'pos'}'$sig{'epos'}";

    next if $mapcut{$entry} || $mapcut{$sense};

    if ($mapglo{$sense}) {
	$mapsigs{$sense} = parse_sig($sense)
	    unless $mapsigs{$sense};
	my %mapsig = %{$mapsigs{$sense}};
	@sig{qw/cf gw sense pos epos/} = @mapsig{qw/cf gw sense pos epos/};
	$sig{'lang'} = $data{'baselang'} unless $data{'baselang'} eq 'qpn';
	$sig = serialize_sig($sig);
    } elsif ($mapglo{$entry}) {
	$mapsigs{$entry} = parse_sig($entry)
	    unless $mapsigs{$entry};
	my %mapsig = %{$mapsigs{$entry}};
	@sig{qw/cf gw pos/} = @mapsig{qw/cf gw pos/};
	$sig{'proj'} = $data{'baseproj'};
	$sig{'lang'} = $data{'baselang'} unless $data{'baselang'} eq 'qpn';
	print $outfh serialize_sig($sig), "\n";
    } else {
	if ($data{'baselang'} eq 'qpn') {
	    $sig =~ s/\@.*?\%/\@$data{'baseproj'}\%/; # REMOVED /g
	} else {
	    $sig =~ s/\@.*?:/\@$data{'baseproj'}\%$data{'baselang'}\:/; # REMOVED /g
	}
	print $outfh "$sig\t$inst\n";
    }
}

close($outfh);
close(S);

1;
