#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::JSON;

my %howto = ();
xcl_howtos();

my $projxtf = shift @ARGV;

die "xcl-json.plx: must give project:PQXID on commandline\n"
    unless $projxtf;

my ($project,$PQX) = split(/:/, $projxtf);
my ($four) = ($PQX =~ /^(....)/);
my $xtf = "$ENV{'ORACC'}/bld/$project/$four/$PQX/$PQX.xtf";

die "xcl-json.plx: no such file $xtf\n"
    unless -r $xtf;

my %frags = ();
my @frags = `xsltproc $ENV{'ORACC'}/lib/scripts/xtf-FRAGS.xsl $xtf`;
foreach (@frags) {
    if (/^:-:(.*?)\t(.*)$/) {
	my ($id,$t) = ($1,$2);
	$t =~ s/\s*$//;
	$frags{$id} = $t;
    }
}

#use Data::Dumper; print Dumper \%frags;
#exit 1;

my @in = `cat $xtf | $ENV{'ORACC'}/bin/xns`;
my $xtf_nons = join('', @in);
my $xxtf = load_xml_string($xtf_nons);
my $xcl = $xxtf->getDocumentElement()->lastChild();

if ($xcl && $xcl->localName() eq 'xcl_xcl') {
    ORACC::JSON::iterate($xcl);
}

sub
textfrag {
    my $n = shift;
    my $r = $n->getAttribute('ref');
    if ($r) {
	my $fragret = '';
	if ($frags{$r}) {
	    my $f = $frags{$r};
	    if ($f =~ /^#l\s+(\S+)\s+(.*)$/) {
		my($n,$label) = ($1,$2);
		my $jn = ORACC::JSON::jsonify($n);
		my $jlabel = ORACC::JSON::jsonify($label);
		$fragret = "\"n\"=\"$jn\",\n\"label\"=\"$jlabel\"";
	    } else {
		my $subtype = $f =~ s/^\@//;
		my $nam = ($subtype ? 'subtype' : 'frag');
		my $val = ORACC::JSON::jsonify($f);
		$fragret = "\"$nam\": \"$val\"";
	    }
	    return $fragret;
	}
    } else {
	return undef;
    }
}

sub
xcl_howtos {
    $howto{'xcl_xcl'} = { type=>"{",nam=>'type',val=>'cdl',att=>'-file langs',chld=>['cdl','['] };
    $howto{'xcl_c'} = { type=>"{",nam=>'node',val=>'c',att=>'-bracketing_level level',
			chld=>['cdl','['] };
    $howto{'xcl_d'} = { type=>"{",nam=>'node',val=>'d',att=>'',hook=>\&textfrag };
    $howto{'xcl_ll'} = { type=>"{",nam=>'node',val=>'ll',att=>'',
			 hook=>\&textfrag,chld=>['choices','['] };
    $howto{'xcl_l'} = { type=>"{",nam=>'node',val=>'l',att=>'',hook=>\&textfrag };
    $howto{'xff_f'} = { nam=>'f',val=>'{',att=>'' };
    $howto{'xcl_linkbase'} = { type=>"{",nam=>'linkbase',val=>'[',att=>'' };
    $howto{'xcl_linkset'} = { type=>"{",nam=>'type',val=>'linkset',att=>'',chld=>['links','['] };
    $howto{'xcl_link'} = { type=>"{",nam=>'type',val=>'link',att=>'' };
    $howto{'xcl_mds'} = { type=>'#ignore' };
    $howto{'xcl_m'} = { type=>'#ignore' };
    $howto{'xcl_props'} = { nam=>'props',val=>'[' };
    $howto{'xcl_prop'} = { type=>'{',nam=>'#ignore',val=>'#ignore',att=>'' };
    $howto{'xcl_para'} = { type=>'#ignore' };
    $howto{'xcl_p'} = { type=>'#ignore' };
    ORACC::JSON::setHowTos(%howto);
}

1;
