#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2P0::L2Super;
use ORACC::L2GLO::Util;

#
# Retrieve sigs from from-xtf-glo.sig because that gives only the instances
# that are in a project's edited texts: if we use project.sig we can get
# things from proxies which would create duplication with superglossaries.
#

my %data = ORACC::L2P0::L2Super::init();

my %mapglo = %{$data{'mapgloref'}};
my %mapsigs = ();

my $mapproj = $data{'project'};
my $maplang = $data{'lang'};
my $outfh = $data{'output_fh'};

my $projsigs = "$ENV{'ORACC'}/bld/$mapproj/from-xtf-glo.sig";
my $projdate = (stat($projsigs))[9];

super_die("can't read $projsigs")
    unless open(S, $projsigs);

my $mapdate = (stat($data{'input'}))[9];

# if 
#   a) the map file is newer than the sigfile we must rebuild
# or
#   b) the sigfile is older than the project sigfile we must rebuild

if (!$data{'force'} && defined $data{'outputdate'}) {
    if ($mapdate < $data{'outputdate'} && $projdate < $data{'outputdate'}) {
	super_warn("$data{'output'} is up to date");
	exit 0;
    }
} else {
    undef $outfh;
    open($outfh, '>', $data{'output'});
}

chatty("importing sigs from $data{'project'}/$data{'lang'}");

### ADD CHECK FOR REFERENCED/REDUNDANT map/fix ENTRIES

while (<S>) {
    next if /^\@(project|name|lang)\s/ || /^\s*$/;
    next unless /\%$data{'baselang'}/;
    chomp;

    my($sig,$inst) = split(/\t/,$_);
    next unless $inst;

    # For now, just deal with entry/sense level mapping
    my %sig = parse_sig($sig);
    my $entry = "$sig{'cf'}\[$sig{'gw'}\]$sig{'pos'}";
    my $sense = "$sig{'cf'}\[$sig{'gw'}//$sig{'sense'}\]$sig{'pos'}'$sig{'epos'}";
    if ($mapglo{$sense}) {
	$mapsigs{$sense} = parse_sig($sense)
	    unless $mapsigs{$sense};
	my %mapsig = %{$mapsigs{$sense}};
	@sig{qw/cf gw sense pos epos/} = @mapsig{qw/cf gw sense pos epos/};
	$sig{'lang'} = $data{'baselang'};
	$sig = serialize_sig($sig);
    } elsif ($mapglo{$entry}) {
	$mapsigs{$entry} = parse_sig($entry)
	    unless $mapsigs{$entry};
	my %mapsig = %{$mapsigs{$entry}};
	@sig{qw/cf gw pos/} = @mapsig{qw/cf gw pos/};
	$sig{'proj'} = $data{'baseproj'};
	$sig{'lang'} = $data{'baselang'};
	print $outfh serialize_sig($sig), "\n";
    } else {
	$sig =~ s/\@.*?:/\@$data{'baseproj'}\%$data{'baselang'}\:/g;
	print $outfh "$sig\t$inst\n";
    }
}

close($outfh);
close(S);

1;
