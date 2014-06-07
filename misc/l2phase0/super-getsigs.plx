#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2P0::L2Super;

my %data = ORACC::L2P0::L2Super::init();
close($data{'input_fh'};

my %mapglo = %{$data{'mapgloref'}};
my %mapsigs = ();

my $mapproj = $data{'project'};
my $maplang = $data{'lang'};
my $outfh = $data{'output_fh'};

my $projsigs = "$ENV{'ORACC'}/bld/$maplang/corpus.sig"; ## I know this is wrong ...
my $projdate = (stat($projsigs))[9];
super_die("can't read $projsigs")
    unless open(S, $projsigs);

my $mapdate = (stat($data{'input'}))[9];

# if 
#   a) the map file is newer than the sigfile we must rebuild
# or
#   b) the sigfile is older than the project sigfile we must rebuild

super_warn "$data{'outputfile'} is up to date" and exit 0
    unless ($mapdate > $data{'outputdate'} || $data{'outputdate'} < $projdate);

while (<S>) {
    chomp;
    my($sig,$inst) = (/^(.*?)\t(.*?)/);
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
	$sig{'lang'} = $data{'baselang'};
	print $outfh serialize_sig($sig);
    } else {
	$sig =~ s/\%(.*?):/$data{'baselang'}/;
	print $outfh "$sig\t$inst";
    }
}

close($outfh);
close(S);

1;
