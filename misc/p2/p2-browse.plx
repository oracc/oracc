#!/usr/bin/perl
use warnings; use strict;
use utf8; use open 'utf8'; binmode(STDOUT, ':utf8');
use lib '/usr/local/oracc/lib';
use ORACC::P2::Util2;
use ORACC::XML;
use Encode;
use constant NO_LIST=>1;
use Getopt::Long;

my %param = ();
GetOptions(
    'param=s' => \%param,
    );

my $bin = '/usr/local/oracc/bin';
my $list = '';
my $oracc = '/usr/local/oracc';
my $res = undef;
my $serialize = 1;
my $session = '';
my $tmpdir = '';
my %values = ();
my $verbose = 1;
my $xdata = undef;
my $xsl = "$oracc/lib/scripts";

$xdata = ORACC::P2::Util2::decode_args_or_cgi();

if ($xdata) {
    my $payload = $xdata->getDocumentElement()->localName();
    if ($payload ne 'browse') {
	die "p2-browse.plx: wrong XML payload; found `$payload' but expected 'browse')\n";
    }
    %values = ORACC::P2::Util2::set_values($xdata)
}

# values on command line override those in an xml file
foreach my $p (keys %param) {
    $values{$p} = $param{$p};
}

($session,$list,$tmpdir) 
    = ORACC::P2::Util2::setup_tmpdir($xdata,\%values);

unless ($xdata) {
    $xdata = fake_browse();
    die "p2-browse.plx: failed to load or fake instance data\n"
	unless $xdata;
    %values = ORACC::P2::Util2::set_values($xdata);
}

#use Data::Dumper; warn Dumper(\%values);

my $browse_list = '';
if ($values{'glossary'} && $values{'glossary'} ne '#none') {
    ORACC::P2::Util2::set_data($xdata,'glossary',$values{'glossary'});
} elsif ($values{'xis-id'}) {
    xsystem("$oracc/bin/xis2", '-o', "$tmpdir/results.lst",
	   '-f', "$oracc/bld/$values{'project'}/$values{'xis-lang'}/$values{'xis-lang'}.xis",
	   '-i', $values{'xis-id'});
    $browse_list = "$values{'project'}/$values{'xis-lang'}/$values{'xis-lang'}.xis\#$values{'xis-id'}";
} elsif ($values{'adhoc'}) {
    open(HOC,">$tmpdir/results.lst");
    print HOC join("\n",split(/,/,$values{'adhoc'})),"\n";
    close(HOC);
    $values{'glossary'} = '#none';
    $browse_list = 'adhoc';
} else {
    $values{'glossary'} = '#none';
    if (!$values{'browse-list'} || $values{'browse-list'} eq '_all') {
	$browse_list = "$oracc/pub/$values{'project'}/cat/pqids.lst";
    } elsif ($values{'browse-list'} =~ /^$oracc/o) {
	$browse_list = $values{'browse-list'};
    } else {
	$browse_list = "$oracc/www/$values{'project'}/lists/$values{'browse-list'}";
    }
    if (-s $browse_list) {
	xsystem('cp', $browse_list, "$tmpdir/results.lst");
    } else {
	xsystem('touch', "$tmpdir/results.lst");
    }
}

ORACC::P2::Util2::set_data($xdata,'browse-list',$browse_list);
ORACC::P2::Util2::save_instance($tmpdir,$xdata);

if ($browse_list) {
    fake_results_xml();
} else {
    fake_results_xml(NO_LIST);
}

my $return_transform = load_xsl("$xsl/oas-RETURN.xsl");
$res = eval { $return_transform->transform($xdata, 
					   results=>"'$tmpdir/results.xml'",
					   search=>"''") };
unless ($res) {
    print STDERR "oas-RETURN.xsl failed\n";
    print STDERR "---\nTransform reports:\n$@\n---\n";
    exit 1; # return error
} else {
    print STDERR " ok\n" if $verbose;
}

$serialize = 0 if $values{'serialize'} eq 'false';
ORACC::P2::Util2::emit_instance($tmpdir, $res, 'browse', $serialize);

1;

#######################################################

sub
fake_browse {
    my %newdata = (
	session=>$session || '',
	list=>$list || '',
	project=>$values{'project'} || '',
	glossary=>$values{'glossary'} || '',
	'serialize'=>$values{'serialize'} || '',
	'adhoc'=>$values{'adhoc'} || '',
	'browse-list'=>$values{'browse-list'} || '',
	'xis-lang'=>$values{'xis-lang'} || '',
	'xis-id'=>$values{'xis-id'} || '',
	);
    $xdata = ORACC::P2::Util2::load_default_instance('browse');
    ORACC::P2::Util2::set_data($xdata,%newdata);
    $xdata;
}

sub
fake_results_xml {
    my $no_list = shift;
    my $wc = ($no_list ? '0' : `wc -l $tmpdir/results.lst`);
    my $first = ($no_list ? '' : `head -1 $tmpdir/results.lst`);
    my $index = '';
    $wc =~ s/\s+.*$//;
    chomp($wc);
    if (!$first) {
	$index = 'cbd';
    } elsif ($first =~ '_/') {
	$index = 'tra';
    } elsif ($first =~ tr/././ > 1) {
	$index = 'txt';
    } else {
	$index = 'cat';
    }
    open(R, ">$tmpdir/results.xml");
    print R "<ret><index>$index</index><dir>$list</dir><count>$wc</count></ret>";
    close R;
}

sub
xsystem {
    warn "system @_\n"
	if $verbose;
    system @_;
}
