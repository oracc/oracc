#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::JSON;

glossary_howtos();

my $projcbd = shift @ARGV;
my ($project,$lang) = split(/:/, $projcbd);

#my $cbd_ns = "$ENV{'ORACC'}/bld/$project/$lang/$lang.xml";
my $cbd_ns = "$ENV{'ORACC'}/bld/$project/$lang/articles-with-periods.xml";

exit 1 unless -r $cbd_ns;

my $xis_ns = "$ENV{'ORACC'}/bld/$project/$lang/$lang.xis";
my $sum_ns = "$ENV{'ORACC'}/www/$project/cbd/$lang/summaries.html";

print "{\n";
print "\t\"type\": \"glossary\",\n";
print "\t\"project\": \"$project\",\n";
print "\t\"lang\": \"$lang\",\n";

my @in = `cat $cbd_ns | $ENV{'ORACC'}/bin/xns`;
my $cbd_nons = join('', @in);
my $xcbd = load_xml_string($cbd_nons);
ORACC::JSON::iterate($xcbd->getDocumentElement());
$xcbd = undef;
$cbd_nons = undef;
print "\n,\n";

ORACC::JSON::reset();

@in = `cat $xis_ns | $ENV{'ORACC'}/bin/xns`;
my $xis_nons = join('',@in);
my $xxis = load_xml_string($xis_nons);
ORACC::JSON::iterate($xxis->getDocumentElement());
$xxis = undef;
print "\n,\n";

ORACC::JSON::reset();

@in = `cat $sum_ns | $ENV{'ORACC'}/bin/xns`;
my $sum_nons = join('',@in);
my $xsum = load_xml_string($sum_nons);
ORACC::JSON::iterate($xsum->getDocumentElement());
$xsum = undef;

print "\n}\n";

###########################################################

sub htmlstring {
    my $n = shift;
    my $tmp = $n->toString();
    $tmp =~ s/xhtml_//g;
    $tmp;
}

sub
glossary_howtos {
    my %howto = ();
    my %attmap = ();

    $howto{'cbd_entries'} = { nam=>"entries", val=>'[' };
    $howto{'cbd_articles'} = { nam=>"entries", val=>'[' };
    $howto{'cbd_entry'} = { type=>"{", nam=>'headword',val=>'@n', att=>'-n' };
    $howto{'cbd_cf'} = { nam=>'cf',val=>'text()' };
    $howto{'cbd_dcf'} = { nam=>'dcf',val=>'text()' };
    $howto{'cbd_gw'} = { nam=>'gw',val=>'text()' };
    $howto{'cbd_pos'} = { nam=>'pos',val=>'text()' };
    $howto{'cbd_epos'} = { nam=>'epos',val=>'text()' };
    $howto{'cbd_mng'} = { nam=>'mng',val=>'text()' };
    $howto{'cbd_forms'} = { nam=>'forms',val=>'[' };
    $howto{'cbd_form'} = { type=>"{",nam=>'type',val=>'form',att=>'' };
    $howto{'cbd_form-sanss'} = { nam=>'form-sanss',val=>'[' };
    $howto{'cbd_form-sans'} = { type=>"{",nam=>'type',val=>'form-sans',att=>'' };
    $howto{'cbd_cof-form-norm'} = { nam=>'cof-form-norm',val=>'text()',att=>'' };
    $howto{'cbd_norms'} = { nam=>'norms',val=>'[' };
    $howto{'cbd_norm'} = { nam=>'#ignore',val=>'{',att=>'' };
    $howto{'cbd_n'} = { nam=>'n',val=>'text()',att=>'' };
    $howto{'cbd_f'} = { type=>"{",nam=>'type',val=>'normform',att=>'' };
    $howto{'cbd_bases'} = { nam=>'bases',val=>'[' };
    $howto{'cbd_base'} = { type=>"{",nam=>'type',val=>'base',att=>'' };
    $howto{'cbd_stems'} = { nam=>'stems',val=>'[' };
    $howto{'cbd_stem'} = { type=>"{",nam=>'type',val=>'stem',att=>'' };
    $howto{'cbd_morphs'} = { nam=>'morphs',val=>'[' };
    $howto{'cbd_morph'} = { type=>"{",nam=>'type',val=>'morph',att=>'' };
    $howto{'cbd_morph2s'} = { nam=>'morph2s',val=>'[' };
    $howto{'cbd_morph2'} = { type=>"{",nam=>'type',val=>'morph2',att=>'' };
    $howto{'cbd_conts'} = { nam=>'conts',val=>'[' };
    $howto{'cbd_cont'} = { type=>"{",nam=>'type',val=>'cont',att=>'' };
    $howto{'cbd_compound'} = { nam=>'compound',val=>'[' };
    $howto{'cbd_cpd'} = { type=>"{",nam=>'type',val=>'cpd',att=>'' };
    $howto{'cbd_senses'} = { nam=>'senses',val=>'[' };
    $howto{'cbd_sense'} = { type=>"{",nam=>'type',val=>'sense',att=>'' };
    $howto{'cbd_sigs'} = { nam=>'sigs',val=>'[' };
    $howto{'cbd_sig'} = { type=>"{",nam=>'type',val=>'sig',att=>'' };
    $howto{'cbd_cof-data'} = { nam=>'cof-data',val=>'{',att=>'' };
    $howto{'cbd_cof-head'} = { nam=>'head',val=>'text()',att=>'0' };
    $howto{'cbd_cof-tail'} = { nam=>'tail',val=>'{',att=>'',text=>'sig' };

    $howto{'cbd_periods'} = { nam=>'periods', val=>'[' };
    $howto{'cbd_p'} = { type=>'{', nam=>'p', val=>"text()", att=>'' };

    $howto{'cbd_props'} = { nam=>'props',val=>'{' };
    $howto{'cbd_prop'} = { nam=>'@key',val=>'[',att=>'-key' };
    $howto{'cbd_v'} = { nam=>'#ignore',val=>'text()',att=>'' };

    $howto{'cbd_pl_id'} = { nam=>'pl_id',val=>'text()',att=>'' };
    $howto{'cbd_pl_coord'} = { nam=>'pl_coord',val=>'text()',att=>'' };    
    
    $howto{'cbd_letter'} = { type=>'#ignore' };
    $howto{'cbd_equivs'} = { type=>'#ignore', recurse=>'no' };
    $howto{'cbd_bib'} = { type=>'#ignore', recurse=>'no' };
    $howto{'cbd_s'} = { type=>'#ignore', recurse=>'no' };
    $howto{'cbd_t'} = { type=>'#ignore', recurse=>'no' };
    $howto{'cbd_text'} = { type=>'#ignore', recurse=>'no' };
    
    $howto{'xis_xisses'} = { nam=>'instances', val=>'{' };
    $howto{'xis_xis'} = { nam=>'@xml:id', val=>"[", att=>'0' };
    $howto{'xis_r'} = { nam=>'#ignore', val=>'text()' };
    $howto{'xis_periods'} = { type=>'#ignore' };

    $howto{'xhtml_html'} = { type=>'#ignore' };
    $howto{'xhtml_head'} = { type=>'#ignore', recurse=>'no' };
    $howto{'xhtml_div'} = { type=>'#ignore' };
    $howto{'xhtml_h1'} = { type=>'#ignore' };
    $howto{'xhtml_body'} = { nam=>'summaries', val=>'{' };
    $howto{'xhtml_p'} = { nam=>'@id', val=>'hook()', hook=>\&htmlstring, recurse=>'no' };

    $howto{'#skipempty'} = 1;
    
    ORACC::JSON::gdl_howtos(\%howto, \%attmap);
    
    ORACC::JSON::setHowTos(%howto);
    ORACC::JSON::setAttMap(%attmap);
}
1;
