#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::JSON;

my %attmap = ();
my %howto = ();
asl_howtos();

my $output = '';

my $asl = shift @ARGV;
$output = shift @ARGV;

die "asl-json.plx: must give signlist on commandline\n"
    unless $asl;

die "asl-json.plx: no such file $asl\n"
    unless -r $asl;

my @in = `cat $asl | $ENV{'ORACC'}/bin/xns`;
my $asl_nons = join('', @in);
my $xasl = load_xml_string($asl_nons);

if ($xasl) {
    print "{\n";
    ORACC::JSON::iterate($xasl->getDocumentElement());
    print ",";
    print "\"index\": {\n";
    asl_index($xasl->getDocumentElement()->getElementsByTagName('sl_v'));
    print "}";
    print "\n}";
}

###############################################################################

sub
asl_index {
    my $nv = 0;
    foreach my $v (@_) {
	print "," if $nv++;
	my $sn = $v->parentNode->getAttribute('n');
	print "\"".ORACC::JSON::jsonify($v->getAttribute('n'))."\": \"".ORACC::JSON::jsonify($sn)."\"";
    }
}

sub
asl_fields_used {
    my $doc = shift;
    my %fields = ();
    foreach my $asl ($doc->getElementsByTagName('asl_asl')) {
	foreach my $c ($asl->firstChild()->childNodes()) {
	    ++$fields{$c->nodeName()} if $c->hasChildNodes();
	}
    }
    my $flds = 0;
    print "\"fields\": [";
    foreach my $f (sort keys %fields) {
	print "," if $flds++;
	$f =~ s/^asl_//;
	print "\"$f\"";
    }
    print "]";
}

sub
asl_sortvals {
    my $proj = shift;
    system "$ENV{'ORACC'}/bin/asl-sv-json.plx $ENV{'ORACC_BUILDS'}/$proj/01bld/sortvals.xml";
}

sub
values {
    my $n = shift;
    my @v = tags($n,'','sl_v');
    if ($#v >= 0) {
	my @pv = ();
	foreach my $v (@v) {
	    next unless $v->parentNode() == $n;
	    my $n = $v->getAttribute('n');
	    my $pn = ORACC::JSON::jsonify($n);
	    push @pv, $pn;
	}
	if ($#pv >= 0) {
	    return "\"values\": [".join(',', map { "\"$_\"" } @pv)."]";
	}
    }
}

sub
asl_howtos {
    $howto{'sl_signlist'} = { type=>"",nam=>'type',val=>'signlist',chld=>['signs','{'],
			   hook=>\&ORACC::JSON::default_metadata
    };

    $howto{'sl_sign'} = { type=>"",nam=>'@n',val=>'{',att=>'-id n', hook=>\&values };
    $howto{'sl_form'} = { type=>"",nam=>'@n',val=>'{',att=>'-id n', hook=>\&values };
    $howto{'sl_name'} = { type=>'', nam=>'gdl', val=>'text()', chld=>['gdl','['], att=>'-id' };
    $howto{'sl_inote'} = { type=>"#ignore" };
    $howto{'sl_note'} = { type=>"#ignore" };
    $howto{'sl_proof'} = { type=>"#ignore" };
    # The values go in the "values:" member of the sign/form
    $howto{'sl_v'} = { type=>"#ignore", recurse=>'no' };
    $howto{'sl_qs'} = { type=>"#ignore", recurse=>'no' };
    
    $howto{'#auto'} = 1;
    $howto{'#skipempty'} = 1;
    
    $attmap{'gdl_utf8'} = '#ignore';

    # note that the XCL version of the gdl howtos retains IDs because they could be useful
    # tracking where graphemes are used.  In the SL context, the IDs are artificial so we
    # just drop them
    my $gdlattr = '-gdl_status gdl_accented id';

    $howto{'gdl_a'} = { type=>'{' , nam=>'a', val=>'text()', att=>$gdlattr };
    $howto{'gdl_b'} = { type=>'{' , nam=>'b', val=>'text()', att=>$gdlattr };
    $howto{'gdl_c'} = { type=>'{',  nam=>'c', val=>'@form', chld=>['seq','['], att=>$gdlattr.' form' };
    $howto{'gdl_d'} = { type=>'{' , nam=>'det', val=>'d', att=>'', chld=>['seq','[']};
    $howto{'gdl_f'} = { type=>'{' , nam=>'f', val=>'text()', att=>$gdlattr };
    $howto{'gdl_g'} = { type=>'{' , nam=>'g', val=>'text()', att=>$gdlattr, chld=>['seq','[']};
    $howto{'gdl_m'} = { type=>'{' , nam=>'m', val=>'text()', att=>$gdlattr };
    $howto{'gdl_n'} = { type=>'{' , nam=>'n', val=>'n', chld=>['seq','['], att=>$gdlattr };
    $howto{'gdl_o'} = { type=>'{' , nam=>'o', val=>'@gdl_type', att=>$gdlattr.' gdl_type' };
    $howto{'gdl_p'} = { type=>'{',  nam=>'p', val=>'@gdl_type', chld=>['punct','['], att=>$gdlattr.' gdl_type' };
    $howto{'gdl_q'} = { type=>'{',  nam=>'q', val=>'@form', chld=>['qualified','['], att=>$gdlattr.' form' };
    $howto{'gdl_r'} = { type=>'{' , nam=>'r', val=>'text()', chld=>['mods','['], att=>$gdlattr };
    $howto{'gdl_s'} = { type=>'{' , nam=>'s', val=>'text()', chld=>['mods','['], att=>$gdlattr };
    $howto{'gdl_v'} = { type=>'{' , nam=>'v', val=>'text()', chld=>['mods','['], att=>$gdlattr };
    $howto{'gdl_w'} = { type=>'' , nam=>'#ignore', val=>'#ignore', att=>'-id',chld=>['signs','['] };
    $howto{'gdl_x'} = { type=>'{' , nam=>'x', val=>'@gdl_type', att=>'-gdl_type'.' '.$gdlattr };
    $howto{'gdl_gg'} = { type=>'{', nam=>'gg',val=>'gg', chld=>['group','['], att=>$gdlattr };
    
    ORACC::JSON::setHowTos(%howto);
    ORACC::JSON::setAttMap(%attmap);
}

1;
