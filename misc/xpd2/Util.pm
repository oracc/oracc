package ORACC::XPD::Util;
use lib '@@ORACC@@/lib';
use ORACC::XML;

my $loaded = 0;
my %config = ();
my %option = ();
my @sources = ();

my $project = '';

sub
all_options {
    keys %option;
}

sub
unload {
    $loaded = 0;
    %options = ();
    %config = ();
}

sub
load_xpd {
    my $argfile = shift;
    my $conf = $argfile || "@@ORACC@@/xml/$project/config.xml";
    my @conf = ();
    if (!-r $conf) {
	push @conf, $conf;
	$conf = '00lib/config.xml'
	    unless $arg_dir; # This tries $arg_dir/00lib/config.xml twice when $arg_dir != ''
	if (!-r $conf) {
	    push @conf, $conf unless $arg_dir;
	    die "XPD::Util: no configuration file, tried: @conf\n";
	}
    }
    my $x = load_xml($conf);
    die "XPD::Util: failed to load configuration $conf\n"
	unless $x;
    $config{'project'} = $x->getDocumentElement()->getAttribute('n');
    foreach my $p ($x->getDocumentElement()->childNodes()) {
	next unless $p->isa('XML::LibXML::Element');
	if ($p->localName() eq 'option') {
	    my $name = $p->getAttribute('name');
	    my $value = $p->getAttribute('value');
	    $option{$name} = $value;
	    if ($name eq 'build-proxies') {
		@sources = grep($_->isa('XML::LibXML::Element'),
				$p->childNodes());
	    }
	} else {
	    $config{$p->localName()} = $p->textContent();
	}
    }
    undef $x;
    ++$loaded;
}

sub
option {
    load_xpd() unless $loaded;
    $config{$_[0]} || $option{$_[0]};
}

sub
project {
    if (-d '00lib') {
	## FIXME: this may not work when ORACC_HOME includes a symbolic link;
	## pwd will report the underlying path and the match will fail.
	my $p = `pwd`;
	chomp $p;
	$p =~ m#^$ENV{'ORACC_HOME'}/(.*?)$#;
	$1;
    } else {
	undef;
    }
}

sub
set_project {
    $project = shift;
}

sub
sources {
    @sources;
}

1;
