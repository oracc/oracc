#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::JSON;

my %attmap = ();
my %howto = ();
sv_howtos();

my $output = '';

my $sv = shift @ARGV;
$output = shift @ARGV;

die "xmd-sv-json.plx: must give catalogue on commandline\n"
    unless $sv;

die "xmd-json.plx: no such file $sv\n"
    unless -r $sv;

my $xsv = load_xml($sv);

if ($xsv) {
    print "{ \"sortvals\": {";
    my $nflds = 0;
    foreach my $field ($xsv->getDocumentElement()->childNodes()) {
	if ($field->hasChildNodes()) {
	    my $n = $field->nodeName();
	    print ",\n" if $nflds++;
	    my $jn = ORACC::JSON::jsonify($n);
	    print "\"$jn\": {\n";
	    my $nvs = 0;
	    foreach my $v ($field->childNodes()) {
		my $n=$v->getAttribute('n');
		my $c=$v->getAttribute('c');
		print ",\n" if $nvs++;
		$jn = ORACC::JSON::jsonify($n);
		print "\"$jn\": \"$c\"";
	    }
	    print "}\n";
	}
    }
    print "}}";
}

###############################################################################

sub
sv_howtos {
    $howto{'sortvals'} = { type=>"{",nam=>'type',val=>'sortvals',att=>'-file langs' }; #,chld=>['members','{']
#    $howto{'xmd_xmd'} = { type=>"",nam=>'@xml:id',val=>'{',att=>'-id' };
#    $howto{'xmd_cat'} = { type=>"#ignore" };    
    $howto{'#auto'} = 1;
    $howto{'#skipempty'} = 1;

    $attmap{'c'} = '#ignore';
#    $attmap{'project'} = '#ignore';
    
    ORACC::JSON::setHowTos(%howto);
    ORACC::JSON::setAttMap(%attmap);
}

1;
